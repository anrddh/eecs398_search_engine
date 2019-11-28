#include <fb/bloom_filter.hpp>
#include <fb/vector.hpp>

#include "doctest.h"

TEST_CASE ("Bloom Filter Test 1") {
    BloomFilter<20,4800,fb::Vector> filter;

    fb::String str1("str1");
    fb::String str2("str2");
    fb::String str3("str3");
    fb::String str4("str4");

    filter.insert(str1);
    CHECK( ! filter.tryInsert( str3 ) );
    CHECK( filter.mightContain( str1 ) );
    CHECK( filter.mightContain( str3 ) );

    /*
	f := NewWithEstimates(1000, 0.001)
	n1 := "Love"
	n2 := "is"
	n3 := "in"
	n4 := "bloom"
	f.AddString(n1)
	n3a := f.TestAndAddString(n3)
	n1b := f.TestString(n1)
	n2b := f.TestString(n2)
	n3b := f.TestString(n3)
	f.TestString(n4)
	if !n1b {
		t.Errorf("%v should be in.", n1)
	}
	if n2b {
		t.Errorf("%v should not be in.", n2)
	}
	if n3a {
		t.Errorf("%v should not be in the first time we look.", n3)
	}
	if !n3b {
		t.Errorf("%v should be in the second time we look.", n3)
	}
    */
}
