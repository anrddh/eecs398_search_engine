#pragma once

#include "isr.hpp"
#include "document_isr.hpp"

class PhraseInfo : public IndexInfo
   {
public:
   PhraseInfo(Location startLoc_, Location endLoc_) : startLoc(startLoc_), endLoc(endLoc_) { }
   ~PhraseInfo( ) { }

   virtual Location GetStartLocation( )
      {
      return startLoc;
      }

   virtual Location GetEndLocation( )
      {
      return endLoc;
      }

private:
   Location startLoc, endLoc;

   };

class PhraseISR : public ISR
   {
public:
   PhraseISR(fb::Vector<fb::UniquePtr<WordISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR);
   ~PhraseISR( ) { }
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual bool AtEnd( );
   virtual uint32_t GetDocumentId( );

private:
   void updateLocationInfo();

   fb::Vector<fb::UniquePtr<WordISR>> Words;
   fb::UniquePtr<DocumentISR> DocIsr;
   Location farthestLocation;
   bool isAtEnd;
   };

PhraseISR::PhraseISR(fb::Vector<fb::UniquePtr<WordISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR)
: Words(std::move(ISRs)), DocIsr(std::move(documentISR)), isAtEnd(false)
   {
      Seek( 1 );
   }

fb::UniquePtr<IndexInfo> PhraseISR::Next( )
   {
   if( isAtEnd )
      {
      return fb::UniquePtr<IndexInfo>( );
      }


   fb::UniquePtr<IndexInfo> info = Words[0]->GetCurrentInfo( );
   Location nearestLocation = info->GetStartLocation( );
   for( fb::SizeT i = 1; i < Words.size( ); ++i )
      {
      info = Words[i]->GetCurrentInfo( );
      if( info->GetStartLocation( ) < nearestLocation )
         {
         nearestLocation = info->GetStartLocation( );
         }
      }

   return Seek( nearestLocation + 1 );
   }

fb::UniquePtr<IndexInfo> PhraseISR::NextDocument( )
   {
   return Seek( DocIsr->Seek( farthestLocation )->GetStartLocation( ) );
   }

fb::UniquePtr<IndexInfo> PhraseISR::Seek( Location target )
   {
   // 1. Seek all ISRs to the first occurrence beginning at the target location.
   fb::UniquePtr<IndexInfo> info = Words[0]->Seek( target );
   if( !info )
      {
      isAtEnd = true;
      return fb::UniquePtr<IndexInfo>( );
      }

   fb::SizeT farthestTerm = 0;
   farthestLocation = info->GetStartLocation( );

   for( fb::SizeT i = 1; i < Words.size( ); ++i )
      {
      info = Words[i]->Seek( target );
      if( !info )
         {
         isAtEnd = true;
         return fb::UniquePtr<IndexInfo>( );
         }

      if( info->GetStartLocation( ) > farthestLocation )
         {
         farthestLocation = info->GetStartLocation( );
         farthestTerm = i;
         }
      }

   // 2. Pick the furthest term and attempt to seek all
   //    the other terms to the first location beginning where they should appear relative to the furthest term.
   fb::SizeT successfulTerms = 0;
   while( successfulTerms != Words.size( ) )
      {
      successfulTerms = 0;
      for( fb::SizeT i = 0; i < Words.size( ); ++i )
         {
         if(i == farthestTerm)
            {
            ++successfulTerms;
            }
         else
            {
            Location target = farthestLocation + ( i - farthestTerm );
            info = Words[i]->Seek(target);

            if( !info )
               {
               isAtEnd = true;
               return fb::UniquePtr<IndexInfo>( );
               }

            Location result = info->GetStartLocation( );
            if( target != result )
               {
               farthestLocation = result;
               farthestTerm = i;
               break;
               }
            ++successfulTerms;
            }
         } 
      }

      return GetCurrentInfo( );
   }

fb::UniquePtr<IndexInfo> PhraseISR::GetCurrentInfo( )
   {
   if ( isAtEnd )
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   else
      {
      return fb::makeUnique<PhraseInfo>(farthestLocation - ( Words.size( ) - 1), farthestLocation );
      }
   }

bool PhraseISR::AtEnd( )
   {
   return isAtEnd;
   }

uint32_t PhraseISR::GetDocumentId( )
   {
   DocIsr->Seek( farthestLocation );
   return DocIsr->GetDocumentId( );
   }

void PhraseISR::updateLocationInfo( )
   {
   bool isFirst = true;
   for(fb::UniquePtr<WordISR> &word : Words )
      {
      fb::UniquePtr<IndexInfo> info = word->GetCurrentInfo( );
      if(!info)
         {
         isAtEnd = true;
         return;
         }
      if( isFirst || info->GetStartLocation( ) < farthestLocation )
         {
         farthestLocation = info->GetStartLocation( );
         }
      
      isFirst = false;
      }

   isAtEnd = false;
   }