#pragma once

#include "isr.hpp"
#include "document_isr.hpp"

class OrInfo : public IndexInfo
   {
   public:
   OrInfo(Location loc_) : loc(loc_) { }
   ~OrInfo( ) { }

   virtual Location GetStartLocation( )
      {
      return loc;
      }

   virtual Location GetEndLocation( )
      {
      return loc;
      }

private:
   Location loc;

   };

class OrISR : public ISR
   {
public:
   OrISR(fb::Vector<fb::UniquePtr<ISR>> ISRs,
         fb::UniquePtr<DocumentISR> documentISR);
   ~OrISR( ) { }
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual bool AtEnd( );
   virtual uint32_t GetDocumentId( );

private:
   void updateLocationInfo( );

   fb::Vector<fb::UniquePtr<ISR>> Terms;
   fb::UniquePtr<DocumentISR> DocIsr;
   Location nearestLocation;
   fb::SizeT nearestTerm;
   bool isAtEnd;
   };

OrISR::OrISR(fb::Vector<fb::UniquePtr<ISR>> ISRs, fb::UniquePtr<DocumentISR> documentISR)
: Terms( std::move( ISRs ) ), DocIsr( std::move( documentISR ) ), isAtEnd(false)
   {
   isAtEnd = true;
   for( fb::UniquePtr<ISR> &term : Terms )
      {
      isAtEnd &= term->AtEnd( );
      }

   if( !isAtEnd )
      {
      updateLocationInfo( );
      }

   }

fb::UniquePtr<IndexInfo> OrISR::Next( )
   {
   if(isAtEnd)
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   Terms[nearestTerm]->Next( );
   updateLocationInfo( );
   return GetCurrentInfo( );
   }

fb::UniquePtr<IndexInfo> OrISR::NextDocument( )
   {
   if(isAtEnd)
      {
      std::cerr << "isAtEnd = true" << std::endl;
      return fb::UniquePtr<IndexInfo>( );
      }

   fb::UniquePtr<IndexInfo> docLoc = DocIsr->Seek( nearestLocation );
   Location skipToLoc = docLoc->GetStartLocation( );

   return Seek( skipToLoc );
   }

fb::UniquePtr<IndexInfo> OrISR::Seek( Location target )
   {
   fb::UniquePtr<IndexInfo> info;
   bool foundMatch = false;
   for( fb::SizeT i = 0; i < Terms.size( ); ++i )
      {
      info = Terms[i]->Seek( target );
      if( info && ( !foundMatch || info->GetStartLocation( ) < nearestLocation ) )
         {
         nearestLocation = info->GetStartLocation( );
         nearestTerm = i;
         foundMatch = true;
         }
      }
   isAtEnd = !foundMatch;

   return GetCurrentInfo( );
   }

fb::UniquePtr<IndexInfo> OrISR::GetCurrentInfo( )
   {
   if( isAtEnd )
      {
      return fb::UniquePtr<IndexInfo>( );
      }

   return fb::makeUnique<OrInfo>(nearestLocation);
   }

bool OrISR::AtEnd( )
   {
   return isAtEnd;
   }

uint32_t OrISR::GetDocumentId( )
   {
   DocIsr->Seek( nearestLocation );
   return DocIsr->GetDocumentId( );
   }

void OrISR::updateLocationInfo( )
   {
   fb::SizeT i = 0;
   isAtEnd = true;
   for( ; i < Terms.size( ); ++i )
      {
      fb::UniquePtr<IndexInfo> info = Terms[i]->GetCurrentInfo( );
      if( info )
         {
         nearestLocation = info->GetStartLocation( );
         nearestTerm = i;
         isAtEnd = false;
         ++i;
         break;
         }
      }

   for( ; i < Terms.size( ); ++i )
      {
      fb::UniquePtr<IndexInfo> info = Terms[i]->GetCurrentInfo( );
      if( info && info->GetStartLocation( ) < nearestLocation )
         {
         nearestLocation = info->GetStartLocation( );
         nearestTerm = i;
         }
      }
   }
