#include <parse/parser.hpp>

fb::NoDeleteUnorderedMap<fb::String, fb::String> Parser::characterConversionMap;
fb::NoDeleteUnorderedSet<fb::StringView> Parser::boldTags;
Parser::InitParser Parser::p;
