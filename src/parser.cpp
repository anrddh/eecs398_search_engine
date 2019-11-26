#include <parse/parser.hpp>

fb::UnorderedMap<fb::String, fb::String> Parser::characterConversionMap;
fb::UnorderedSet<fb::StringView> Parser::boldTags;
Parser::InitParser Parser::p;
