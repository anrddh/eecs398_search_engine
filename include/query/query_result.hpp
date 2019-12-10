//  Created by Jaeyoon Kim on 11/10/19.
#pragma once
#include <fb/priority_queue.hpp>
#include <fb/mutex.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <atomic>

// TODO add ranking info
struct QueryResult{
    fb::SizeT UrlId;
    double rank;
    fb::String Title;
    fb::String Snippet;
}

// Stores top n pages
// Thread safe ( highly concurrent )
class TopPages {
   TopPages( int n_ ) : n(n_) {};
   void addRankStats( QueryResult& result ) {
      if ( result.rank < min_allowed_rank )
         return;

      fb::AutoLock l(mtx);
      // TODO make rank pair
      top.push( result );
      if ( top.size() <= n )
         return;

      topPair.pop();
      min_allowed_rank = topPair.top();
   }

   fb::Vector<QueryResult> GetTopResults( )
      {
      fb::Vector<QueryResult> results;
      while( topPair.size( ) )
         {
         results.pushBack(topPair.top( ) );
         topPair.pop( );
         }

      return results;
      } 
private:

   fb::PriorityQueue<QueryResult> topPair;
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

struct QueryResult{
    fb::SizeT UrlId;
    double rank;
    fb::String Title;
    fb::String Snippet;
}
