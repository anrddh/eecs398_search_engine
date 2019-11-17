// * Created by Jaeyoon Kim
// * Edited by Aniruddh Agarwal to improve conformance with style and
//   memory map frontier

#include "frontier_pool.hpp"
#include "DiskVec.hpp"

#include "../../lib/mutex.hpp"
#include "../../lib/utility.hpp"

#include <atomic>
#include <iostream>

#include <stdlib.h>

using fb::SizeT;
using fb::Pair;
using fb::Vector;
using fb::AutoLock;

using std::atomic;

// TODO set to 0?
atomic<int> insertCounter = 0;
atomic<int> getCounter = 0;
atomic<int> randSeedCounter = 0;

constexpr SizeT NUM_BINS = 16; // This indicates how many seperate UrlFrontierBin there are
constexpr SizeT NUM_TRY = 1000;
constexpr SizeT NUM_SAMPLE = 3;

class UrlFrontierBin {
public:
   /* TODO: FIGURE OUT HOW TO STATICALLY GENERATE FILE NAMES */
   UrlFrontierBin( )
       : localSeed( ++randSeedCounter ),
         idx( localSeed ),
         toParse( "frontier" ) {}

   void addUrl(const FrontierUrl &url) {
      AutoLock lock( toParseM );
      toParse.pushBack( url );
   }

   Vector<SizeT> getUrl( ) {
      int rand_num[ NUM_TRY ];
      localSeedM.lock( );
      for ( int i = 0;  i < NUM_TRY;  ++i )
         rand_num[ i ] = rand_r( &localSeed );
      localSeedM.unlock( );

      Vector<SizeT> urls_to_return;
      AutoLock lock( toParseM );
      if (toParse.size( ) < NUM_SAMPLE)
         return {}; // empty url

      int max_ranking = 0; // Requires that any ranking of urls to be greater than 0
      int max_idx;

      // Find what to sample
      // Compute the highest ranking amongst first NUM_SAMPLE randomly picked urls
      for (int i = 0; i < NUM_SAMPLE; ++i) {
         if ( max_ranking < toParse[ rand_num[i] % toParse.size() ].ranking ) {
            max_ranking = toParse[ rand_num[i] % toParse.size() ].ranking;
            max_idx = rand_num[i] % toParse.size();
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
      for ( int i = NUM_SAMPLE; i < NUM_TRY && !toParse.empty(); ++i ) {
         if ( toParse[ rand_num[i] % toParse.size() ].ranking >= max_ranking ) {
            urls_to_return.pushBack( toParse[ rand_num[i] % toParse.size() ].offset ) ;
            toParse[ rand_num[i] % toParse.size() ] = toParse.back();
            toParse.popBack( );
         }
      }

      return urls_to_return;
   }

private:
   fb::Mutex localSeedM;
   fb::Mutex toParseM;
   unsigned int localSeed;
   unsigned int idx;
   DiskVec<FrontierUrl> toParse;
};

UrlFrontierBin frontiers[ NUM_BINS ];

void frontierAddUrl(const FrontierUrl &url) {
   frontiers[ (++insertCounter) % NUM_BINS ].addUrl(url);
}

Vector<SizeT> frontierGetUrls() {
   return frontiers[ (++getCounter) % NUM_BINS ].getUrl();
}
