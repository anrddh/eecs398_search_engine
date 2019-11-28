#pragma once

#include "index_reader.hpp"
#include "isr.hpp"

class DocumentISR : WordISR 
   {
public:
   unsigned GetDocumentLength( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );

private:
   friend class IndexReader;

   DocumentISR(char * location, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS); 

   };

DocumentISR::DocumentISR(char * location, int NUM_SKIP_TABLE_BITS, int MAX_TOKEN_BITS) 
: WordISR(location, fb::UniquePtr<DocumentISR>( ), NUM_SKIP_TABLE_BITS, MAX_TOKEN_BITS) { }

unsigned int DocumentISR::GetDocumentLength( )
   {
   uint32_t delta;
   fb::read_word_post(currentLocation, delta);
   return delta - 1;
   }

virtual fb::UniquePtr<IndexInfo> NextDocument( )
   {
   return Next( );
   }