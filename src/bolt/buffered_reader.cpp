#include "bolt/buffered_reader.hpp"

BufferedReader::BufferedReader(const char *buf, int len)
   : front((char *)buf), current(buf), length(len) {}

BufferedReader::BufferedReader(const std::unique_ptr<char[]> &buf, int len)
   : front(buf.get()), current(buf.get()), length(len) {}

fb::String BufferedReader::nextLine() 
   {
   movePastWhitespace();
   setStartOfOutput();
   moveToNewLine();
   return createOutputString();
   }

fb::String BufferedReader::nextWord() 
   {
   makeSureBufferHeadIsInBuffer();
   moveToNextLetter();
   setStartOfOutput();
   moveToNextWhitespace();
   return createOutputString();
   }

bool BufferedReader::pastEnd() { return current < front + length; }

void BufferedReader::skipLine() { moveToNewLine(); }

void BufferedReader::resetHead() { current = front; }

void BufferedReader::makeSureBufferHeadIsInBuffer() 
   {
   if (current >= front + length) 
      {
      throw fb::Exception("tried to read past the end of a buffer.");
      }
   }

void BufferedReader::movePastWhitespace() 
   {
   while ((*current == ' ' || *current == '\t') && current - front < length) 
      {
      ++current;
      }
   }

void BufferedReader::moveToNextLetter() 
   {
   while ((*current == ' ' || *current == '\t' || *current == '\n') &&
            current - front < length) 
      {
      ++current;
      }
   }

void BufferedReader::moveToNewLine() 
   {
   while ((*(current) != '\n') && current - front < length) 
      {
      ++current;
      }
   }

void BufferedReader::setStartOfOutput() { stringStart = current; }

void BufferedReader::moveToNextWhitespace() {
  while (*current != ' ' && *current != '\n' && *current != '\t' &&
         current - front < length) 
      {
      ++current;
      }
   }

fb::String BufferedReader::createOutputString() 
   {
  const char *endPosition = current;
  ++current;
  return fb::String(stringStart,
                     fb::min(endPosition, front + length) - stringStart);
   }
