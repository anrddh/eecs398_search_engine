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
    std::cout << "Query: " << p.stream.input << "END QUERY\n";
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 6" ) {
    fb::String stream = "unsigned long variables";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 7" ) {
    fb::String stream = "b \\ b";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 8" ) {
    fb::String stream = "a b \\ c | d";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 9" ) {
    fb::String stream = "a ( b \\ c ) | d";
    QueryParser p(stream);
    std::cout << "Stream: " << p.stream.input << '\n';
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 10" ) {
    fb::String stream = "a ( b )";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 11" ) {
    fb::String stream = "a ()";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(!static_cast<bool>(expr));
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 12" ) {
    fb::String stream = "a ((b))";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 13" ) {
    fb::String stream = "\"a\"";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 14" ) {
    fb::String stream = "\"nicole hamilton\"";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 15" ) {
    fb::String stream = "(nicole | hamilton)";
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 16" ) {
    fb::String stream = "nicole | (hamilton human)";
    std::cout << "NEW QUERY: " << stream << '\n';
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 17" ) {
    fb::String stream = "(nicole | hamilton human)";
    std::cout << "NEW QUERY: " << stream << '\n';
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 18" ) {
    fb::String stream = "human (nicole | hamilton)";
    std::cout << "NEW QUERY: " << stream << '\n';
    QueryParser p(stream);
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}

TEST_CASE( "QueryQueryParser Test 19" ) {
    fb::String stream = "(nicole | hamilton) human";
    QueryParser p(stream);
    std::cout << "NEW QUERY: " << p.stream.input << '\n';
    auto expr = p.Parse();
    CHECK(static_cast<bool>(expr));
    expr->Print();
    std::cout << "\n\n\n\n";
}
