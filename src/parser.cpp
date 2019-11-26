#include <parse/parser.hpp>

fb::UnorderedMap<fb::String, fb::String> Parser::characterConversionMap;
fb::UnorderedSet<fb::String> Parser::boldTags;
Parser::InitParser Parser::p;
