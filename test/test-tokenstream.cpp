#include <parse/tokenstream.hpp>

#include "doctest.h"

TEST_CASE( "Tokenstream Test 1" ) {
    fb::String stream = "cat dog";
    TokenStream s(stream);
    auto word1 = s.ParseWord();
    CHECK( s.Match( ' ' ) );
    auto word2 = s.ParseWord();
    CHECK(word1 == "cat"_sv);
    CHECK(word2 == "dog"_sv);
}

TEST_CASE( "Tokenstream Test 2" ) {
    fb::String stream = "cat &dog";
    TokenStream s(stream);
    auto word1 = s.ParseWord();
    CHECK( s.Match( ' ' ) );
    auto word2 = s.ParseWord();
    CHECK(word1 == "cat"_sv);
    CHECK(word2 == "dog"_sv);
}

TEST_CASE( "Tokenstream Test 3" ) {
    fb::String stream = "cat \\ dog";
    TokenStream s(stream);
    auto word1 = s.ParseWord();
    CHECK( s.Match( '\\' ) );
    auto word2 = s.ParseWord();
    CHECK(word1 == "cat"_sv);
    CHECK(word2 == "dog"_sv);
}

TEST_CASE( "Tokenstream Test 4" ) {
    fb::String stream = "(cat & dog) \\ \"bear\"";
    TokenStream s(stream);
    CHECK( s.Match( '(' ) );
    auto word1 = s.ParseWord();
    CHECK( s.Match( ' ' ) );
    auto word2 = s.ParseWord();
    CHECK( s.Match( ')' ) );
    CHECK( s.Match( '\\' ) );
    CHECK( s.Match( '"' ) );
    auto word3 = s.ParseWord();
    CHECK( s.Match( '"' ) );
    CHECK(word1 == "cat"_sv);
    CHECK(word2 == "dog"_sv);
    CHECK(word3 == "bear"_sv);
}
