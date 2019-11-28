// * Created by Jaeyoon Kim
// * Edited by Aniruddh Agarwal to improve conformance with style and
//   memory map frontier

#include <disk/frontier.hpp>
#include <disk/disk_vec.hpp>

#include <fb/mutex.hpp>
#include <fb/utility.hpp>
#include <fb/string.hpp>
#include <fb/string_view.hpp>

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

using std::atomic;

// TODO set to 0?
atomic<int> insertCounter = 0;
atomic<int> getCounter = 0;
atomic<int> randSeedCounter = 0;


FrontierBin::FrontierBin(StringView filename)
    : localSeed( ++randSeedCounter ),
      toParse(filename) {}

void FrontierBin::addUrl(const FrontierUrl &url) {
    AutoLock lock( toParseM );
    toParse.pushBack( url );
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

void Frontier::addUrl(const FrontierUrl &url) {
    FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
    ptr[ (++insertCounter) % NumFrontierBins ].addUrl(url);
}

Vector<SizeT> Frontier::getUrl() const {
    FrontierBin *ptr = reinterpret_cast<FrontierBin *>(frontiers);
    return ptr[ (++getCounter) % NumFrontierBins ].getUrl();
}
