//  Created by Jaeyoon Kim on 12/10/19.
#pragma once
#include <fb/priority_queue.hpp>
#include <fb/mutex.hpp>
#include <fb/string.hpp>
#include <fb/stddef.hpp>
#include <tcp/url_tcp.hpp>
#include <atomic>
#include <iostream>

struct QueryResult {
    fb::SizeT UrlId;
    fb::String Title;
    fb::String Snippet;
    double rank;

    inline bool operator< ( const QueryResult& other ) const {
      return rank < other.rank;
    }
};

// T needs to have a double named rank with non-negative values
// And define operator< based on rank
template <typename T>
class TopNQueue {
public:
    TopNQueue( fb::SizeT n_ ) : n( n_ ) {}
    void push( T&& v ) {
        std::cout << "adding object with ranking " << v.rank << std::endl;
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

    inline void reset() {
        fb::AutoLock l(mtx);
        min_allowed_rank = 0;
        topQueue.clear();
    }

private:
   fb::PriorityQueue<T> topQueue;
   fb::Mutex mtx;
   std::atomic<double> min_allowed_rank = 0;
   fb::SizeT n;
};


// Stores top n pages
// Thread safe ( highly concurrent )
// Just keep adding pages
// When done, delete it (or just let it go out of scope)
// In the destructor, will send the top pages to master (in order)
//
// TCP protocol method:
// worker to master:
// num (int), [ UrlId (uint64_t), Title (string), Snippet (string), rank (double) ] x num
// The rank will be increasing
//
class TopPages {
public:
    TopPages( int n ) : top( n ) {};
    void add( QueryResult&& result ) {
        top.push( std::move( result ) );
    }

    void send_and_reset( int sock ) {
        try {
            std::cout << "sending int of size " << top.size() << 
                " in send and reset" << std::endl; // TODO delete
            send_int( sock, top.size() );
            std::cout << "sent int in send and reset" << std::endl; // TODO delete
            while ( !top.empty() ) {
                std::cout << "popping" << std::endl;
                std::cout << top.top().UrlId << std::endl;
                std::cout << top.top().Title<< std::endl;
                std::cout << top.top().Snippet << std::endl;
                send_query_result( sock, top.top() );
                top.pop();
            }
        } catch(...) {
            top.reset();
            throw;
        }
        top.reset();
    }

    fb::SizeT size() const {
        return top.size();
    }

   // TODO I don't think this every needs to be called? Uncommenting this for local use of query server
   fb::Vector<QueryResult> GetTopResults( )
      {
      fb::Vector<QueryResult> results;
      while( !top.empty( ) )
         {
         results.pushBack( top.top( ) );
         top.pop( );
         }

      return results;
      }

private:
   // Destructor helper
   inline void send_query_result( int sock, const QueryResult& qr ) {
      send_uint64_t( sock, qr.UrlId );
      send_str( sock, qr.Title );
      send_str( sock, qr.Snippet );
      send_double( sock, qr.rank );
   }

   TopNQueue<QueryResult> top;
};
