#include "frontier_pool.hpp"
#include "../../lib/mutex.hpp"
#include "../../lib/utility.hpp"
#include <atomic>

#include <iostream>

#include <stdlib.h>

using fb::SizeT;
using fb::Pair;

using std::atomic;

// TODO set to 0?
atomic<int> insert_counter = 0;
atomic<int> get_counter = 0;
atomic<int> rand_seed_counter = 0;

constexpr int NUM_BINS = 16; // This indicates how many seperate UrlFrontierBin there are
constexpr int NUM_TRY = 1000;
constexpr int NUM_SAMPLE = 3;

class UrlFrontierBin {
public:
   UrlFrontierBin() {
      local_seed = ++rand_seed_counter;
   }

   void add_url( SizeT url_offset, SizeT url_ranking ) {
      to_parse_m.lock();
      to_parse.pushBack( Pair<SizeT, SizeT>{url_offset, url_ranking} );
      to_parse_m.unlock();
   }

   Vector<SizeT> get_url() {
       int rand_num[NUM_TRY];
       local_seed_m.lock();
       for (int i = 0; i < NUM_TRY; ++i) {
           rand_num[i] = rand_r(&local_seed);
       }
       local_seed_m.unlock();

       int max_ranking = 0; // Requires that any ranking of urls to be greater than 0
       int max_idx;

       Vector< SizeT > urls_to_return;
       to_parse_m.lock();
       if (to_parse.size() < NUM_SAMPLE) {
           to_parse_m.unlock();
           return {}; // empty url
       }

       // Find what to sample
       // Compute the highest ranking amongst first NUM_SAMPLE randomly picked urls
       for (int i = 0; i < NUM_SAMPLE; ++i) {
           if ( max_ranking < to_parse[ rand_num[i] % to_parse.size() ].second ) {
               max_ranking = to_parse[ rand_num[i] % to_parse.size() ].second;
               max_idx = rand_num[i] % to_parse.size();
           }
       }


       urls_to_return.pushBack( to_parse[max_idx].first );
       to_parse[ max_idx ] = to_parse.back();
       to_parse.popBack();

       // We randomly check urls
       // If their ranking is greater than or equal to max_ranking,
       // then we will take them to be parsed
       // Note that it is possible that same url might be checked multiple times
       // However, this is not likely since there should be many urls in here each time
       for ( int i = NUM_SAMPLE; i < NUM_TRY && !to_parse.empty(); ++i ) {
           if ( to_parse[ rand_num[i] % to_parse.size() ].second >= max_ranking )
           {
               urls_to_return.pushBack( to_parse[ rand_num[i] % to_parse.size() ].first ) ;
               to_parse[ rand_num[i] % to_parse.size() ] = to_parse.back();
               to_parse.popBack();
           }
       }

       to_parse_m.unlock();
       return urls_to_return;
   }

   // TODO delete
   void print_debug() {
      for (auto p : to_parse) {
         std::cout << p.first << std::endl;
      }
   }

private:
   Mutex local_seed_m;
   unsigned int local_seed;
   Mutex to_parse_m;
   Vector< Pair<SizeT, SizeT> > to_parse;
};

UrlFrontierBin frontiers[ NUM_BINS ];

void frontier_add_url(SizeT url_offset, SizeT url_ranking) {
   int local_counter = ++insert_counter;
   frontiers[ local_counter % NUM_BINS ].add_url( url_offset, url_ranking );
}

Vector<SizeT> frontier_get_url() {
   int local_counter = ++get_counter;
   return frontiers[ local_counter % NUM_BINS ].get_url();
}
