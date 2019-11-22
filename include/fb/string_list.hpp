#pragma once

#include "stdio.h"

#include "vector.hpp"
#include "string.hpp"

namespace fb
{

class StringList 
   {
public:
   StringList() : numChars(0) { }

   StringList& operator<<(const fb::String &word) 
      {
      numChars += word.size();
      strings.pushBack(word);
      return *this;
      }

   StringList& operator<<(const char *word)
      {
      strings.emplaceBack(word);
      numChars += strings.back().size();

      return *this;
      }

   StringList& operator<<(char letter)
      {
      if(strings.size() && strings.back().size() < 64)
         {
         strings.back().pushBack(letter);
         }
      else 
         {
         strings.emplaceBack(1, letter);
         }
      ++numChars;

      return *this;
      }

   fb::String str() 
      {
      fb::String out('a', numChars);
      int currentChar = 0;
      for(fb::String str : strings) 
         {
         memcpy(out.data() + currentChar, str.data(), str.size());
         currentChar += str.size();
         }
      return out;
      }

private:
   int numChars;
   fb::Vector<fb::String> strings;

   };

}