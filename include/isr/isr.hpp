#pragma once

#include "fb/memory.hpp"
#include "fb/indexEntry.hpp"

typedef uint32_t Location;

class ISR
   {
public:
   virtual fb::UniquePtr<IndexInfo> Next( ) = 0;
   virtual fb::UniquePtr<IndexInfo> NextDocument( ) = 0;
   virtual fb::UniquePtr<IndexInfo> Seek( Location target ) = 0;
   virtual uint32_t GetDocumentId( ) = 0;
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( ) = 0;
   virtual bool AtEnd( ) = 0;
   virtual ~ISR( ) { }
   };
