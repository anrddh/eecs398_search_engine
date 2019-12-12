#include <fb/string.hpp>

// The thing we send back to master!
struct PageResult {
    fb::StringView Url;
    fb::StringView Title;
    fb::StringView Snippet;
    double rank;

    inline bool operator< ( const PageResult& other ) const {
      return rank < other.rank;
    }
};
