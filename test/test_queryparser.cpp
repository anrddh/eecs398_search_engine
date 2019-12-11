#include <parse/query_parser.hpp>

#include "doctest.h"

TEST_CASE( "QueryQueryParser Test 1" ) {
    fb::String stream = "cat dog";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 2" ) {
    fb::String stream = "cat \\ dog";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 3" ) {
    fb::String stream = "cats \\ \"Alex\"";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 4" ) {
    fb::String stream = "cats \\ dogs bears";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 5" ) {
    fb::String stream = "cats \\ (dogs bears)";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 5" ) {
    fb::String stream = "unsigned long variables";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 6" ) {
    fb::String stream = "b \\ b";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 6" ) {
    fb::String stream = "a b \\ c | d";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 6" ) {
    fb::String stream = "a ( b \\ c ) | d";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 7" ) {
    fb::String stream = "a ( b )";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 8" ) {
    fb::String stream = "a ()";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(!static_cast<bool>(expr));
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 9" ) {
    fb::String stream = "a ((b))";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}
