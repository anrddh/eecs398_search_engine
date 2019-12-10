#pragma once

#include "isr.hpp"

class WordISR : public ISR
   {
public:
   ~WordISR( ) { }
   virtual unsigned int GetDocumentCount( ) = 0;
   virtual unsigned int GetNumberOfOccurrences( ) = 0;
   virtual uint32_t GetDocumentId( ) = 0;
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( ) = 0;
   virtual fb::UniquePtr<IndexInfo> Next( ) = 0;
   virtual fb::UniquePtr<IndexInfo> NextDocument( ) = 0;
   virtual fb::UniquePtr<IndexInfo> Seek( Location target ) = 0;
   virtual bool AtEnd( ) = 0;
   };