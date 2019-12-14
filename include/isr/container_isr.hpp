#pragma once

#include "isr.hpp"
#include "document_isr.hpp"

class ContainerInfo : public IndexInfo
   {
public:
   ContainerInfo(Location startLoc_, Location endLoc_) : startLoc(startLoc_), endLoc(endLoc_) { }
   ~ContainerInfo( ) { }

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

class ContainerISR : public ISR
   {
public:
   ContainerISR(fb::UniquePtr<ISR> IncludeISR,
                fb::UniquePtr<ISR> ExcludeISR,
                fb::UniquePtr<DocumentISR> documentISR);
   ~ContainerISR( ) { }
   virtual fb::UniquePtr<IndexInfo> Next( );
   virtual fb::UniquePtr<IndexInfo> NextDocument( );
   virtual fb::UniquePtr<IndexInfo> Seek( Location target );
   virtual fb::UniquePtr<IndexInfo> GetCurrentInfo( );
   virtual bool AtEnd( );
   virtual uint32_t GetDocumentId( );

private:
   fb::UniquePtr<ISR> IncludeIsr, ExcludeIsr;
   fb::UniquePtr<DocumentISR> DocIsr;
   bool isAtEnd;
   };

ContainerISR::ContainerISR(fb::UniquePtr<ISR> IncludeISR, fb::UniquePtr<ISR> ExcludeISR, fb::UniquePtr<DocumentISR> documentISR)
: IncludeIsr( std::move( IncludeISR ) ), ExcludeIsr( std::move( ExcludeISR ) ), DocIsr( std::move( documentISR ) ), isAtEnd(false)
   {
   Seek( 1 );
   }

fb::UniquePtr<IndexInfo> ContainerISR::Next( )
   {
   if( isAtEnd )
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   else
      {
      return Seek( IncludeIsr->GetCurrentInfo( )->GetStartLocation( ) + 1 );
      }
   }

fb::UniquePtr<IndexInfo> ContainerISR::NextDocument( )
   {
   if( isAtEnd )
      {
      return fb::UniquePtr<IndexInfo>( );
      }
   else
      {
      return Seek( DocIsr->Seek( IncludeIsr->GetCurrentInfo( )->GetStartLocation( ) )->GetStartLocation( ) );
      }
   }

fb::UniquePtr<IndexInfo> ContainerISR::Seek( Location target )
   {
   Location endDocLocation = 0;
   Location excludeLoc = 0;

   while(excludeLoc <= endDocLocation)
      {
      fb::UniquePtr<IndexInfo> includeInfo = IncludeIsr->Seek( target );
      if( !includeInfo )
         {
         isAtEnd = true;
         return fb::UniquePtr<IndexInfo>( );
         }

      fb::UniquePtr<IndexInfo> DocInfo = DocIsr->Seek( includeInfo->GetStartLocation( ) );
      endDocLocation = DocInfo->GetStartLocation( );
      Location startDocLocation = endDocLocation - DocIsr->GetDocumentLength( );

      fb::UniquePtr<IndexInfo> excludeInfo = ExcludeIsr->Seek( startDocLocation );
      if( !excludeInfo )
         {
         return includeInfo;
         }

      excludeLoc = excludeInfo->GetStartLocation( );
      target = DocIsr->Seek( excludeLoc )->GetStartLocation( ) + 1;
      }

   return GetCurrentInfo( );
   }

fb::UniquePtr<IndexInfo> ContainerISR::GetCurrentInfo( )
   {
   if( isAtEnd )
      {
      std::cout << "isAtEnd: " << isAtEnd << std::endl;
      return fb::UniquePtr<IndexInfo>( );
      }
   else
      {
      fb::UniquePtr<IndexInfo> info = IncludeIsr->GetCurrentInfo( );
      return fb::makeUnique<ContainerInfo>( info->GetStartLocation( ), info->GetEndLocation( ) );
      }
   }

bool ContainerISR::AtEnd( )
   {
   return isAtEnd;
   }

uint32_t ContainerISR::GetDocumentId( )
   {
   DocIsr->Seek( GetCurrentInfo( )->GetStartLocation( ) );
   return DocIsr->GetDocumentId( );
   }
