#pragma once

#include "isr.hpp"
#include "document_isr.hpp"

class AndInfo : public IndexInfo
   {
public:
   AndInfo(Location startLoc_, Location endLoc_) : startLoc(startLoc_), endLoc(endLoc_) { }
   ~AndInfo( ) { }

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

class AndISR : public ISR
   {
public:
   AndISR(fb::Vector<fb::UniquePtr<ISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR);
   ~AndISR( ) { }
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual bool AtEnd( );
   virtual uint32_t GetDocumentId( );

private:
   void seekAllPast(Location target);
   void updateLocationInfo();

   fb::Vector<fb::UniquePtr<ISR>> Terms;
   fb::UniquePtr<DocumentISR> DocIsr;
   Location nearestStartLocation, nearestEndLocation, farthestStartLocation, farthestEndLocation;
   bool isAtEnd;
   };


AndISR::AndISR( fb::Vector<fb::UniquePtr<ISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR ) 
: Terms( std::move( ISRs ) ), DocIsr( std::move( documentISR ) ), isAtEnd(false)
   {
   updateLocationInfo( );

   for(fb::UniquePtr<ISR> &isr : ISRs)
      {
      isAtEnd |= isr->AtEnd( );
      }

   if(!isAtEnd)
      {
      Seek( 1 );
      }
   } 

fb::UniquePtr<IndexInfo> AndISR::Seek( Location target )
   {
   // 1. Seek all the ISRs to the first occurrence beginning at
   // the target location.
   DocIsr->Seek( target );
   seekAllPast( target );

   fb::UniquePtr<IndexInfo> docLoc = DocIsr->GetCurrentInfo( );
   Location docStart = 0;
   while (!isAtEnd && farthestEndLocation > docLoc->GetEndLocation()) 
      {
      // 2. Move the document end ISR to just past the farthest
      // word, then calculate the document begin location.
      docLoc = DocIsr->Seek(farthestEndLocation);

      // 3. Seek all the other terms to past the document begin.
      docStart = DocIsr->GetCurrentInfo()->GetEndLocation() -
                           (DocIsr->GetDocumentLength() - 1);
      seekAllPast(docStart);

      // 4. If any term is past the document end, return to
      // step 2.
      }

   // 5. If any ISR reaches the end, there is no match.
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>();
      }
   else
      {
      return fb::makeUnique<AndInfo>(nearestStartLocation, farthestEndLocation);
      }
   
   }

fb::UniquePtr<IndexInfo> AndISR::Next( )
   {
   return Seek( nearestStartLocation + 1 );
   }

fb::UniquePtr<IndexInfo> AndISR::NextDocument( )
   {
   return Seek( DocIsr->GetCurrentInfo( )->GetEndLocation( ) + 1 );
   }

fb::UniquePtr<IndexInfo> AndISR::GetCurrentInfo( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   else
      {
      return fb::makeUnique<AndInfo>(nearestStartLocation, farthestEndLocation);
      }
   }

bool AndISR::AtEnd( )
   {
   return isAtEnd;
   }

uint32_t AndISR::GetDocumentId( )
   {
   return DocIsr->GetDocumentId( );
   }

void AndISR::seekAllPast(Location target)
   {
   for(fb::SizeT i = 0; i < Terms.size( ); ++i)
      {
      fb::UniquePtr<IndexInfo> info = Terms[i]->Seek(target);

      if(!info) 
         {
         isAtEnd = true;
         return;
         }
      
      }
      updateLocationInfo();
      
   }

void AndISR::updateLocationInfo( )
   {
      fb::UniquePtr<IndexInfo> info = Terms[0]->GetCurrentInfo( );
      nearestStartLocation = info->GetStartLocation( );
      nearestEndLocation = info->GetEndLocation( );
      farthestStartLocation = info->GetStartLocation( );
      farthestEndLocation = info->GetEndLocation( );
      for( fb::SizeT i = 1; i < Terms.size(); ++i )
         {
         info = Terms[i]->GetCurrentInfo( );
         if(info->GetStartLocation( ) < nearestStartLocation)
            {
            nearestStartLocation = info->GetStartLocation( );
            }

         if(info->GetStartLocation( ) > farthestStartLocation)
            {
            farthestStartLocation = info->GetStartLocation( );
            }

         if(info->GetEndLocation( ) < nearestEndLocation)
            {
            nearestEndLocation = info->GetEndLocation( );
            }

         if(info->GetEndLocation( ) > farthestEndLocation)
            {
            farthestEndLocation = info->GetEndLocation( );
            }
         }
   }
