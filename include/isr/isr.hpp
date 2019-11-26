#pragma once

#include "fb/indexEntry.hpp"

typedef uint64_t Location;

class ISR;

class Post
   {
public:
   virtual Location GetStartLocation( ) = 0;
   virtual Location GetEndLocation( ) = 0;
   };

class IndexReader
   {
public:
   IndexReader(char * startOfIndex);

   ISRWord *OpenISRWord( char *word );
   ISRWord *OpenISREndDoc( );
   };

class ISR
   {
public:
   virtual Post *Next( ) = 0;
   virtual Post *NextDocument( ) = 0;
   virtual Post *Seek( int target );
   virtual int GetStartLocation( );
   virtual int GetEndLocation( );
   };

class ISRWord : public ISR
   {
   public:
      ISRWord(char * location, ISREod documentISR, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS);

      unsigned GetDocumentCount( );
      unsigned GetNumberOfOccurrences( );
      virtual Post *GetCurrentPost( );

   private:
      ISREod docISR;
      const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
      const char * const skipTable;
      const char * currentLocation;
      bool isAtEnd;
   };

char * findSkipTable(char * start) 
   {
   while(*(start++) != '\0')
      ;
   start += (2 * sizeof(unsigned int));
   return start;
   }

ISRWord::ISRWord(char * location, ISREod documentISR, int NUM_SKIP_TABLE_BITS_, int MAX_TOKEN_BITS_) 
   : docISR(documentISR),
      NUM_SKIP_TABLE_BITS(NUM_SKIP_TABLE_BITS_), 
      MAX_TOKEN_BITS(MAX_TOKEN_BITS),
      skipTable(findSkipTable(location)), 
      currentLocation(skipTable + (2 << NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int)),
      isAtEnd(fb::is_word_sentinel(currentLocation))
   { }

virtual Post *ISRWord::Next( ) 
   {
   if(isAtEnd)
      {
         return nullptr;
      }
   Location wordPosition;
   currentLocation = fb::read_word_post(currentLocation, wordPosition);

}