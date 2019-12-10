#include <isr/empty_isr.hpp>

unsigned int EmptyISR::GetDocumentCount( ) { return 0; }
unsigned int EmptyISR::GetNumberOfOccurrences( ) { return 0; }
uint32_t EmptyISR::GetDocumentId( ) { return 0; }
fb::UniquePtr<IndexInfo> EmptyISR::GetCurrentInfo( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::Next( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::NextDocument( ) { return fb::UniquePtr<IndexInfo>( ); }
fb::UniquePtr<IndexInfo> EmptyISR::Seek( Location target ) { return fb::UniquePtr<IndexInfo>( ); }
bool EmptyISR::AtEnd( ) { return true; }
