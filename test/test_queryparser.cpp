#include <parse/query_parser.hpp>

#include "doctest.h"

//TEST_CASE( "Queryparser Test 1" ) {
    //    fb::String stream = "cat dog";
    //    Parser p(stream);
    //    auto expr = p.Parse();
    //    CHECK(static_cast<bool>(expr));
    //    //expr->Eval();
    //}
    //
    //TEST_CASE( "Queryparser Test 2" ) {
    //    fb::String stream = "cat \\ dog";
    //    Parser p(stream);
    //    auto expr = p.Parse();
    //    CHECK(static_cast<bool>(expr));
    //    //expr->Eval();
    //}

TEST_CASE( "Queryparser Test 3" ) {
    fb::String stream = "cats \\ \"Alex\"";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Eval();
}

TEST_CASE( "Queryparser Test 4" ) {
    fb::String stream = "cats \\ dogs bears";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Eval();
}

TEST_CASE( "Queryparser Test 5" ) {
    fb::String stream = "cats \\ (dogs bears)";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Eval();
}
