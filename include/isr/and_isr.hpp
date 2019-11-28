#pragma once

#include "isr.hpp"
#include "document_isr.hpp"

class AndInfo : IndexInfo
   {
   PostInfo(Location startLoc_, Locaiton endLoc_) : startLoc(startLoc_), endLoc(endLoc_) { }

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

class AndISR : ISR
   {
public:
 
   virtual fb::UnqiuePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual bool AtEnd( );

private:
   void seekAllPast(Location target);
   void updateLocationInfo(fb::UniquePtr<IndexInfo> &info);

   fb::Vector<fb::UniquePtr<ISR>> Terms;
   fb::UniquePtr<DocumentISR> DocIsr;
   fb::SizeT nearestTerm, farthestTerm;
   Location furthestLocation;
   Location nearestStartLocation, nearestEndLocation;
   bool isAtEnd;
   };

fb::UniquePtr<IndexInfo> AndISR::Seek( Location target );
   {
   // 1. Seek all the ISRs to the first occurrence beginning at
   // the target location.
   seekAllPast(target);

   
   while( furthestLocation > DocIsr->GetCurrentInfo( )->GetEndLocation( ) || isAtEnd )
      {
      // 2. Move the document end ISR to just past the furthest
      // word, then calculate the document begin location.
      DocIsr->Seek( furthestLocation );
      while( DocIsr->GetCurrentInfo( )->GetEndLocation( ) < furthestLocation ) 
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
      return fb::makeUnique<AndInfo>(nearestStartLocation, furthestLocation);
      }
   
   }

fb::UnqiuePtr<IndexInfo> AndISR::Next( )
   {
   return Seek( nearestStartLocation + 1 );
   }

virtual fb::UniquePtr<IndexInfo> NextDocument( )
   {
   DocIsr->Next( );
   Next( );
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
      
      updateLocationInfo(info);
      }
      
   }

void AndISR::updateLocationInfo(fb::UniquePtr<IndexInfo> &info)
   {
   Location endLoc = info->GetEndLocation( ); 
   Location startLoc = info->GetStartLocation( );
   
   if(endLoc > furthestLocation)
      {
      furthesLocation = endLoc;
      farthestTerm = i;
      }

   if(endLoc < nearestEndLocation)
      {
      nearestEndLocaiton = endLoc;
      }

   if(startLoc < nearestStartLocation)
      {
      nearestStartLocation = startLoc;
      }
   }
