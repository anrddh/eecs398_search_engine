#pragma once

#include <fb/string.hpp>

#include <stdio.h> // perror

#include "isr/word_isr.hpp"
#include "isr/document_isr.hpp"
#include "isr/index_reader_helpers.hpp"

class WordInfo : public IndexInfo
   {
public:
   WordInfo(Location postLoc) : loc(postLoc) { }
   ~WordInfo( ) { }
   virtual Location GetStartLocation( )
      {
      return loc;
      }

   virtual Location GetEndLocation( )
      {
      return loc;
      }

private:
   Location loc;
   };

class DocumentISR;

class WordImplISR : public WordISR
   {
public:
   WordImplISR(const char * location,
               fb::UniquePtr<DocumentISR> documentISR,
               int MAX_TOKEN_BITS);
   WordImplISR(WordImplISR && other) = default;
   ~WordImplISR( ) { }
   virtual unsigned int GetDocumentCount( );
   virtual unsigned int GetNumberOfOccurrences( );
   virtual uint32_t GetDocumentId( );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );
   virtual fb::String GetWord( );

private:
   fb::UniquePtr<DocumentISR> docISR;
   const int NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS;
   uint32_t absolutePosition;
   unsigned int * const skipTable;
   unsigned int * rankingData;
   const char * currentLocation;
   const char * start;
   bool isAtEnd;
   };
