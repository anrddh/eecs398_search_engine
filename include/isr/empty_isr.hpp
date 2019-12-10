#pragma once 

#include <stdio.h> // perror

#include "isr.hpp"
#include "word_isr.hpp"


class EmptyISR : public WordISR
   {
public:
   EmptyISR( ) { }
   ~EmptyISR( ) { } 
   virtual unsigned int GetDocumentCount( ) override;
   virtual unsigned int GetNumberOfOccurrences( ) override;
   virtual uint32_t GetDocumentId( ) override;
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( ) override;
   virtual fb::UniquePtr<IndexInfo> Next( ) override;
   virtual fb::UniquePtr<IndexInfo> NextDocument( ) override;
   virtual fb::UniquePtr<IndexInfo> Seek( Location target ) override;
   virtual bool AtEnd( ) override;
   virtual fb::String GetWord( ) override;
   };

unsigned int EmptyISR::GetDocumentCount( ) { return 0; }
unsigned int EmptyISR::GetNumberOfOccurrences( ) { return 0; }
uint32_t EmptyISR::GetDocumentId( ) { return 0; }
fb::UniquePtr<IndexInfo> EmptyISR::GetCurrentInfo( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::Next( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::NextDocument( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::Seek( Location target ) { return fb::UniquePtr<IndexInfo>( ); }
bool EmptyISR::AtEnd( ) { return true; }
fb::String EmptyISR::GetWord( ) { return fb::String( ); }