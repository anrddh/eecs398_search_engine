// * Created by Jaeyoon Kim
// * Edited by Aniruddh Agarwal to improve conformance with style and
//   memory map frontier

#include <disk/frontier.hpp>
#include <disk/disk_vec.hpp>
#include <disk/url_store.hpp>
#include <disk/UrlInfo.hpp>

#include <fb/mutex.hpp>
#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/utility.hpp>
#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <fb/bloom_filter.hpp>
#include <fb/functional.hpp>

#include <atomic>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>

using fb::SizeT;
using fb::Pair;
using fb::Vector;
using fb::AutoLock;
using fb::StringView;
using fb::String;
using fb::Mutex;
using fb::CV;
using fb::Thread;

using std::atomic;

// TODO set to 0?
atomic<int> insertCounter = 0;
atomic<int> getCounter = 0;
atomic<int> randSeedCounter = 0;

Mutex doShutdownM;
CV doShutdownCV;
bool doShutdown = false;
atomic<int> numBinsShutdown = 0;

FrontierBin::FrontierBin(String filename)
    : localSeed( ++randSeedCounter ),
      toParse(filename), bloom(filename + "_bloom_filter") 
   {
   Thread t( addQueueToToParsed, this );
   t.detach();
   }

void* addQueueToToParsed(void *ptr) 
   {
   while ( true )
      {

      {
      AutoLock lock( doShutdownM );
      if ( doShutdown )
         {
         if ( ++numBinsShutdown == NumFrontierBins )
            {
            doShutdownCV.signal();
            return nullptr;
            }
         }
      }

      reinterpret_cast< FrontierBin* >(ptr)->addToFrontierFromQueue( );
      }
   }

void FrontierBin::addToQueue( fb::Vector< fb::String >&& urls ) 
   {
   AutoLock lock( toAddQueueM );
   toAddQueue.pushBack( std::move( urls ) );
   }

void FrontierBin::addToFrontierFromQueue()
   {
   toAddQueueM.lock();
   Vector< Vector< String > > temp;
   temp.swap( toAddQueue );
   toAddQueueM.unlock();

   while ( !temp.empty() )
      {
      Vector< String > urls = std::move( temp.back() );
      temp.popBack();
      AutoLock lock( toParseM );
      for ( const String& url : urls ) 
         {
         if ( !bloom.tryInsert( url ) )
            continue;

         SizeT url_offset = UrlStore::getStore().addUrl( url );
         SizeT ranking = RankUrl( url );
         toParse.pushBack( {url_offset, ranking} );
         }
      }
   }

void FrontierBin::addSeen(StringView url) 
   {
   bloom.insertWithoutLock( url );
   }

SizeT FrontierBin::size() const
{
   return toParse.size();
}


Vector<SizeT> FrontierBin::getUrl( ) {
    SizeT rand_num[ NUM_TRY ];
    localSeedM.lock( );
    for ( SizeT i = 0;  i < NUM_TRY;  ++i )
        rand_num[ i ] = rand_r( &localSeed );

    SizeT region = rand_r( &localSeed );
    localSeedM.unlock( );

    Vector<SizeT> urls_to_return;
    AutoLock lock( toParseM );
    if (toParse.size( ) < NUM_SAMPLE)
        return {}; // empty url

    SizeT max_ranking = 0; // Requires that any ranking of urls to be greater than 0
    SizeT max_idx = 0;

    // Find what to sample
    // Compute the highest ranking amongst first NUM_SAMPLE randomly picked urls
    for (SizeT i = 0; i < NUM_SAMPLE; ++i) {
       const SizeT idx = search_index(rand_num[i], region);
        if ( max_ranking < toParse[ idx  ].ranking ) {
            max_ranking = toParse[ idx ].ranking;
            max_idx = idx;
        }
    }

    urls_to_return.pushBack( toParse[ max_idx ].offset );
    toParse[ max_idx ] = toParse.back( );
    toParse.popBack( );

    // We randomly check urls
    // If their ranking is greater than or equal to max_ranking,
    // then we will take them to be parsed
    // Note that it is possible that same url might be checked multiple times
    // However, this is not likely since there should be many urls in here each time
    for ( auto i = NUM_SAMPLE; i < NUM_TRY && !toParse.empty(); ++i ) {
        const SizeT idx = search_index(rand_num[i], region);
        if ( toParse[ idx ].ranking >= max_ranking ) {
            urls_to_return.pushBack( toParse[ idx ].offset ) ;
            toParse[ idx ] = toParse.back();
            toParse.popBack( );
        }
    }

    return urls_to_return;
}

Frontier *Frontier::ptr = nullptr;
char Frontier::frontiers[ sizeof(FrontierBin) * NumFrontierBins ];

void Frontier::init(String prefix) {
    std::cout << "Initializing frontier: maximum value of rand() is " << RAND_MAX << std::endl;
    std::cout << "if this is too low, frontier might not behave sufficiently randomly" << std::endl;
    delete ptr;

    ptr = new Frontier;

    FrontierBin *fbptr = reinterpret_cast<FrontierBin *>(frontiers);
    for (SizeT i = 0; i < NumFrontierBins; ++i) {
        auto fname = prefix + fb::toString(i);
        new (fbptr + i) FrontierBin(fname.data());
    }
}

Frontier & Frontier::getFrontier() {
    return *ptr;
}

SizeT Frontier::size() const {
   SizeT total = 0;
   for (SizeT i = 0; i < NumFrontierBins; ++i)
   {
      total += reinterpret_cast<FrontierBin* >(frontiers + i * sizeof(FrontierBin))->size();
   }
   return total;
}

void Frontier::addSeen(StringView url) {
    FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
    ptr[ fb::fnvHash( url.data(), url.size() ) % NumFrontierBins ].addSeen( url );
}

void Frontier::addUrls( Vector< String >&& urls ) 
   {
   static Vector< String > buffers[ NumFrontierBins ];
   for ( SizeT i = 0; i < urls.size(); ++i )
      {
      buffers[ fb::fnvHash( urls[i].data(), urls[i].size() ) % NumFrontierBins ].pushBack( 
            std::move( urls[i] ) );
      }

   for ( SizeT j = 0; j < NumFrontierBins; ++j )
      {
      // This also sets buffers[ j ] to be default vector
      reinterpret_cast<FrontierBin *>(frontiers)[ j ].addToQueue( std::move( buffers[ j ] ) );
      }
   }


Vector<SizeT> Frontier::getUrl() const 
   {
   FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
   return ptr[ (++getCounter) % NumFrontierBins ].getUrl();
   }

void Frontier::shutdown()  
   {
   doShutdown = true;

   doShutdownM.lock();
   while ( numBinsShutdown != NumFrontierBins )
      {
      doShutdownCV.wait( doShutdownM );
      }
      doShutdownM.unlock();
   }
