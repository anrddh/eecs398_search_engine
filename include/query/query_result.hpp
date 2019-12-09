//  Created by Jaeyoon Kim on 11/10/19.
#pragma once
#include <fb/priority_queue.hpp>
#include <fb/mutex.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <atomic>

// TODO add ranking info

// Stores top n pages
// Thread safe ( highly concurrent )
class TopPages {
   TopPages( int n_ ) : n(n_) {};
   void addRankStats( double ranking, rank_stats&& rankingInfo ) {
      if ( ranking < min_allowed_rank )
         return;

      fb::AutoLock l(mtx);
      // TODO make rank pair
      top.push( rankPair );
      if ( top.size() <= n )
         return;

      topPair.pop();
      min_allowed_rank = topPair.top();
   }
private:
   struct RankPair {
      double rank;
      // TODO put data here
   }
   fb::PriorityQueue<RankPair> topPair;
   fb::Mutex mtx;
   std::atomic<double> min_allowed_rank;
   int n;
};

struct SnippetOffsets{
    fb::SizeT begin;
    fb::SizeT end;
}

struct SnippetStats {
    //since we currently think each worker computer will only have one merged
    //pagestore file, we can probably later make this static
    fb::String FileName; //the corresponding pagestore file
    fb::SizeT DocIndex; //gives which number document in that pagestore file
    SnippetOffsets Offsets; //gives the begin and end word number within that document
};
