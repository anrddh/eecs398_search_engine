#include <isr/document_isr.hpp>

DocumentISR::DocumentISR(const char * location, int MAX_TOKEN_BITS_)
: NUM_SKIP_TABLE_BITS( *( unsigned int * ) findSkipTable( location ) ),
   MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
   absolutePosition(0),
   docId(0),
   skipTable( ( ( unsigned int * ) findSkipTable( location ) ) + 1),
   rankingData(skipTable - 2),
   currentLocation( ( const char * ) ( skipTable + ( 1 << NUM_SKIP_TABLE_BITS ) * 2 ) ),
   start(location),
   isAtEnd(fb::is_document_sentinel(currentLocation))
   {
   if(!isAtEnd)
      {
      currentLocation = fb::read_document_post(currentLocation, absolutePosition, docId);
      }
   }

/*
DocumentISR::DoucmentISR( DocumentISR &&other ) =
: NUM_SKIP_TABLE_BITS( other.NUM_SKIP_TABLE_BITS ),
   MAX_TOKEN_BITS( other.MAX_TOKEN_BITS ),
   absolutePosition( other.absolutePosition ),
   docId( other.docId ),
   skipTable( other.skipTable ),
   rankingData( other.rankingData ),
   currentLocation( other.currentLocation ),
   start( other.location ),
   isAtEnd( other.isAtEnd )
   { }
   */

unsigned int DocumentISR::GetDocumentLength( )
   {
   return docLength;
   }

unsigned int DocumentISR::GetDocumentCount( )
   {
   return rankingData[0];
   }

unsigned int DocumentISR::GetDocumentId( )
   {
   return docId;
   }

fb::UniquePtr<IndexInfo> DocumentISR::GetCurrentInfo( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   return fb::makeUnique<DocumentInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> DocumentISR::Next( )
   {
   if(isAtEnd || fb::is_document_sentinel( currentLocation ) )
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   currentLocation = fb::read_document_post(currentLocation, docLength, docId);

   absolutePosition += docLength;
   return fb::makeUnique<DocumentInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> DocumentISR::NextDocument( )
   {
   return Next( );
   }

fb::UniquePtr<IndexInfo> DocumentISR::Seek( Location target )
   {
   isAtEnd = false;
   int index = target >> (MAX_TOKEN_BITS - NUM_SKIP_TABLE_BITS);
   currentLocation = start + skipTable[2 * index];
   absolutePosition = skipTable[2 * index + 1];
   if(!currentLocation)
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>();
      }

   currentLocation = fb::read_document_post(currentLocation, docLength, docId);

   while( absolutePosition < target && Next( ) )
      ;

   return GetCurrentInfo();
   }

bool DocumentISR::AtEnd( )
   {
   return isAtEnd;
   }
