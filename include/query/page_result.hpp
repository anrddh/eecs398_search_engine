#include <fb/string.hpp>
// Made by Jaeyoon

// The thing we send back to master!
struct PageResult {
    fb::String Url;
    fb::String Title;
    fb::String Snippet;
    double rank;

    // We need to flip the order because priority queue has max at top
    inline bool operator< ( const PageResult& other ) const {
      return rank > other.rank;
    }
};
