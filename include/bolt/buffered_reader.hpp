#pragma once

#include <memory> 

#include "fb/algorithm.hpp"
#include "fb/string.hpp"

class BufferedReader 
   {
public:
   explicit BufferedReader(const char *buf, int len);
   explicit BufferedReader(const std::unique_ptr<char[]> &buf, int len);

   fb::String nextLine();

   fb::String nextWord();

   bool pastEnd();

   void skipLine();

   void resetHead();

private:
   void makeSureBufferHeadIsInBuffer();

   void movePastWhitespace();

   void moveToNextLetter();

   void moveToNewLine();

   void setStartOfOutput();

   void moveToNextWhitespace();

   fb::String createOutputString();

   const char *front;
   const char *current;
   const int length;

   const char *stringStart;
   };
