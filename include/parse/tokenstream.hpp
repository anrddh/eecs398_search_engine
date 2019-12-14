/*
 * tokenstream.h
 *
 * Declaration of a stream of tokens that you can read from
 *
 * Lab3: You do not have to modify this file, but you may choose to do so
 */

#ifndef TOKENSTREAM_H_
#define TOKENSTREAM_H_

#include <fb/string.hpp>

#include <algorithm>
#include <sstream>

#include <assert.h>
#include <tolower.h>

// #include "expression.h"

bool CharIsControl(char c) {
    switch ( c ) {
    case '\\':
    case '|':
    case '(':
    case ')':
    case '"':
    case ' ':
        return true;
    }
    return false;
}

bool CharIsRelevant( char c ) {
    return CharIsControl(c) || isalnum(c) || c == ' ';
}

/**
 * Opposite of char is relevant. Needed for filtering the input
 */
bool CharIsIrrelevant( char c ) {
    return !CharIsRelevant(c);
}

/**
 * The token stream, which you can both Match( ) a single character from,
 * or ParseNumber( ) to consume a whole int64_t
 *
 * The input string by default is filtered of any characters that are
 * deemed "irrelevant" by the CharIsIrrelevant function above
 */
class TokenStream {
public:
   // The input we receive, with only relevant characters left
   fb::String input;
   // Where we currently are in the input
    size_t location { 0 };


   /**
    * Construct a token stream that uses a copy of the input
    * that contains only characters relevant to math expressions
    */
   TokenStream(fb::String &in) {
       for (auto &c : in) {
           c = tolower(c);
           if (CharIsIrrelevant(c))
               c = ' ';
       }

       std::istringstream stream(in.data());

       std::string word;
       if (stream >> word)
           input += word.c_str();

       while (stream >> word) {
           if ((!CharIsControl(input.back()) && !CharIsControl(word.front()))
               || (word.front() == '(' && !CharIsControl(input.back())))
               input += ' ';
           input += word.c_str();
       }
   }

   /**
    * Attempt to match and consume a specific character
    *
    * Returns true if the char was matched and consumed, false otherwise
    */
   bool Match(char c) {
       if (AllConsumed())
           return false;

       if (input[location] == c) {
           ++location;
           return true;
       }

       return false;
   }

   /**
    * Check whether all the input was consumed
    */
   bool AllConsumed() const {
       return location >= input.size();
   }

   template <typename Pred>
   fb::StringView ParseUntil(Pred p) {
       if (AllConsumed())
           return "";

       auto begin = location;
       while (!AllConsumed() && !p(input[location]))
           ++location;

       return fb::StringView(input.data() + begin, location - begin);
   }

   fb::StringView ParseUntil(char c) {
       return ParseUntil([c](char d) { return c == d; });
   }

   fb::StringView ParseWord() {
       return ParseUntil([](char c) { return CharIsControl(c); });
   }
};

#endif /* TOKENSTREAM_H_ */
