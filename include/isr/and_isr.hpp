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

private:
   void seekAllPast(Location target);
   void updateLocationInfo();

   fb::Vector<fb::UniquePtr<ISR>> Terms;
   fb::UniquePtr<DocumentISR> DocIsr;
   fb::SizeT nearestStartTerm, nearestEndTerm, farthestStartTerm, farthestEndTerm;
   Location nearestStartLocation, nearestEndLocation, farthestStartLocation, farthestEndLocation;
   bool isAtEnd;
   };


AndISR::AndISR(fb::Vector<fb::UniquePtr<ISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR) : Terms(std::move(ISRs)), DocIsr(std::move(documentISR)) 
   {
   Next( );
   } 

fb::UniquePtr<IndexInfo> AndISR::Seek( Location target )
   {
   // 1. Seek all the ISRs to the first occurrence beginning at
   // the target location.
   seekAllPast(target);

   
   while( farthestEndLocation > DocIsr->GetCurrentInfo( )->GetEndLocation( ) || isAtEnd )
      {
      // 2. Move the document end ISR to just past the farthest
      // word, then calculate the document begin location.
      DocIsr->Seek( farthestEndLocation );
      while( DocIsr->GetCurrentInfo( )->GetEndLocation( ) < farthestEndLocation ) 
         {
         DocIsr->Next( );
         }

      // 3. Seek all the other terms to past the document begin.
      Location docStart = DocIsr->GetCurrentInfo( )->GetEndLocation( ) - DocIsr->GetDocumentLength( );
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
   DocIsr->Next( );
   return Next( );
   }

fb::UniquePtr<IndexInfo> AndISR::GetCurrentInfo( )
   {
   return fb::makeUnique<AndInfo>(nearestStartLocation, farthestEndLocation);
   }

bool AndISR::AtEnd( )
   {
   return isAtEnd;
   }

void AndISR::seekAllPast(Location target)
   {
   for(fb::SizeT i = 0; i < Terms.size( ); ++i)
      {
      fb::UniquePtr<IndexInfo> info = Terms[i]->Seek(target);
      while(info->GetEndLocation( ) < target)
         {
         info = Terms[i]->Next( );
         }

      if(!info) 
         {
         isAtEnd = true;
         return;
         }
      
      updateLocationInfo();
      }
      
   }

void AndISR::updateLocationInfo( )
   {
      fb::UniquePtr<IndexInfo> info = Terms[0]->GetCurrentInfo( );
      nearestStartTerm = info->GetStartLocation( );
      nearestEndTerm = info->GetEndLocation( );
      farthestStartTerm = info->GetStartLocation( );
      farthestEndTerm = info->GetEndLocation( );
      for( fb::SizeT i = 0; i < Terms.size(); ++i )
         {
         info = Terms[i]->GetCurrentInfo( );
         if(info->GetStartLocation( ) < nearestStartTerm)
            {
            nearestStartTerm = info->GetStartLocation( );
            }

         if(info->GetStartLocation( ) > farthestStartTerm)
            {
            farthestStartTerm = info->GetStartLocation( );
            }

         if(info->GetEndLocation( ) < nearestEndTerm)
            {
            nearestEndTerm = info->GetEndLocation( );
            }

         if(info->GetEndLocation( ) < farthestEndTerm)
            {
            farthestEndTerm = info->GetEndLocation( );
            }
         }
   }
