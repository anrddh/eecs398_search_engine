#include <fb/bloom_filter.hpp>
#include <fb/vector.hpp>

#include "doctest.h"

TEST_CASE ("Bloom Filter Test 1") {
    BloomFilter<5,24,fb::Vector> filter;

    fb::String str("test");

    CHECK( filter.tryInsert( str ) );
}
