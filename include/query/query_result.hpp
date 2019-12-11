//  Created by Jaeyoon Kim on 11/10/19.
#pragma once
#include <fb/priority_queue.hpp>
#include <fb/mutex.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <tcp/url_tcp.hpp>
#include <atomic>

// TODO add ranking info
struct QueryResult {
    fb::SizeT UrlId;
    double rank;
    fb::String Title;
    fb::String Snippet;

    inline bool operator< ( const QueryResult& other ) const {
      return rank < other.rank;
    }
};

// T needs to have a double named rank with non-negative values
// And define operator< based on rank
template <typename T>
class TopNQueue {
public:
   TopNQueue( int n_ ) : n( n_ ) {}
   void push( T&& v ) {
      if ( v.rank < min_allowed_rank )
         return;

      fb::AutoLock l(mtx);
      topQueue.push( std::move( v ) );
      if ( topQueue.size() <= n )
         return;

      topQueue.pop();
      min_allowed_rank = topQueue.top().rank;
   }

   inline bool empty() const {
      return topQueue.empty();
   }

   inline fb::SizeT size() const {
      return topQueue.size();
   }

   inline void pop() {
      topQueue.pop();
   }

   inline const T& top() {
      return topQueue.top();
   }

private:
   fb::PriorityQueue<QueryResult> topQueue;
   fb::Mutex mtx;
   std::atomic<double> min_allowed_rank = 0;
   int n;
};


// Stores top n pages
// Thread safe ( highly concurrent )
// Just keep adding pages
// When done, delete it (or just let it go out of scope)
// In the destructor, will send the top pages to master (in order)
//
// TCP protocol method:
// worker to master:
// num (int), [ UrlId (uint64_t), rank (double), Title (string), Snippet (string) ] x num
// The rank will be increasing
//
class TopPages {
   TopPages( int n ) : top( n ) {};
   void addRankStats( QueryResult&& result ) {
      top.push( std::move( result ) );
   }

   ~TopPages() {
      int sock; //TODO
      try {
         send_int( sock, top.size() );
         while ( !top.empty() ) {
            send_query_result( sock, top.top() );
            top.pop();
         }

      } catch (SocketException& se) {
         // If we can't send, just don't do anything
      }
   }

   // TODO I don't think this every needs to be called?
   /*
   fb::Vector<QueryResult> GetTopResults( )
      {
      fb::Vector<QueryResult> results;
      while( topPair.size( ) )
         {
         results.pushBack( topPair.top( ) );
         topPair.pop( );
         }

      return results;
      }
      */

private:
   // Destructor helper
   inline void send_query_result( int sock, const QueryResult& qr ) {
      send_uint64_t( sock, qr.UrlId );
      send_double( sock, qr.rank );
      send_str( sock, qr.Title );
      send_str( sock, qr.Snippet );
   }

   TopNQueue<QueryResult> top;
};
