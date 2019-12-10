#include <parse/query_parser.hpp>

#include "doctest.h"

TEST_CASE( "Queryparser Test 1" ) {
    fb::String stream = "cat dog";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 2" ) {
    fb::String stream = "cat \\ dog";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 3" ) {
    fb::String stream = "cats \\ \"Alex\"";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 4" ) {
    fb::String stream = "cats \\ dogs bears";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 5" ) {
    fb::String stream = "cats \\ (dogs bears)";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 5" ) {
    fb::String stream = "unsigned long variables";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 6" ) {
    fb::String stream = "b \\ b";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 6" ) {
    fb::String stream = "a b \\ c | d";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 6" ) {
    fb::String stream = "a ( b \\ c ) | d";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 7" ) {
    fb::String stream = "a ( b )";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 8" ) {
    fb::String stream = "a ()";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(!static_cast<bool>(expr));
    std::cout << "\n\n\n\n";
}

TEST_CASE( "Queryparser Test 9" ) {
    fb::String stream = "a ((b))";
    Parser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}
