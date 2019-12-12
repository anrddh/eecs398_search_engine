#include "isr/word_impl_isr.hpp"

WordImplISR::WordImplISR(const char * location, fb::UniquePtr<DocumentISR> documentISR, int MAX_TOKEN_BITS_)
   : docISR(std::move(documentISR)),
      NUM_SKIP_TABLE_BITS( *( unsigned int * ) findSkipTable( location ) ),
      MAX_TOKEN_BITS(MAX_TOKEN_BITS_),
      absolutePosition(0),
      skipTable(((unsigned int *)findSkipTable(location)) + 1),
      rankingData(skipTable - 2),
      currentLocation( ( ( const char * ) skipTable ) + ( 1 << NUM_SKIP_TABLE_BITS ) * 2 * sizeof( unsigned int ) ),
      start(location),
      isAtEnd(fb::is_word_sentinel(currentLocation))
   {
   if(!isAtEnd)
      {
      currentLocation = fb::read_word_post(currentLocation, absolutePosition);
      }
   }

unsigned int WordImplISR::GetDocumentCount( )
   {
   return rankingData[1];
   }

unsigned int WordImplISR::GetNumberOfOccurrences( )
   {
   return rankingData[0];
   }

uint32_t WordImplISR::GetDocumentId( )
   {
   return docISR->GetDocumentId( );
   }

fb::UniquePtr<IndexInfo> WordImplISR::GetCurrentInfo( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   return fb::makeUnique<WordInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordImplISR::Next( )
   {
   if(isAtEnd || fb::is_word_sentinel( currentLocation )  )
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   uint32_t delta;
   currentLocation = fb::read_word_post(currentLocation, delta);

   absolutePosition += delta;
   return fb::makeUnique<WordInfo>(absolutePosition);
   }

fb::UniquePtr<IndexInfo> WordImplISR::NextDocument( )
   {
   if( isAtEnd || fb::is_word_sentinel( currentLocation ) )
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   fb::UniquePtr<IndexInfo> docInfo = docISR->Seek( absolutePosition );

   if(!docInfo)
      {
      perror("ERROR, word with no EOD after it");
      }

   return Seek( docInfo->GetStartLocation( ) );
   }

fb::UniquePtr<IndexInfo> WordImplISR::Seek( Location target )
   {
   isAtEnd = false;
   int index = target >> (MAX_TOKEN_BITS - NUM_SKIP_TABLE_BITS);
   currentLocation = start + skipTable[2 * index];
   absolutePosition = skipTable[2 * index + 1];
   if(!absolutePosition)
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>();
      }

   uint32_t trash;
   currentLocation = fb::read_word_post(currentLocation, trash); // move past first element

   while( absolutePosition < target && Next( ) )
      ;

   return GetCurrentInfo( );
   }

bool WordImplISR::AtEnd( )
   {
   return isAtEnd;
   }

fb::String WordImplISR::GetWord( )
   {
   return fb::String(start);
   }