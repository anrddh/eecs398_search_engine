#pragma once

#include "fb/memory.hpp"

#include "isr/isr.hpp"

struct rank_stats
   {
	uint32_t page_store_number;
   uint32_t page_store_index;
	unsigned int total_term_count; // doc length
	fb::Vector<uint32_t> term_freq;
   };

class ConstraintSolver 
   {
public:
   ConstraintSolver( fb::UniquePtr<ISR> isrIn, fb::UniquePtr<DocumentISR> docIsrIn, fb::Vector<fb::UniquePtr<WordISR>> wordIsrsIn, uint32_t page_store_number ) 
   : mainIsr( std::move( isrIn ) ), docIsr( std::move( docIsrIn ) ), wordIsrs( std::move( wordIsrsIn ) ), page_store_num( page_store_number ), doc_frequencies(wordIsrs.size( ), 0)
      { 
      for( fb::SizeT i = 0; i < doc_frequencies.size( ); ++i )
         {
         doc_frequencies[i] = wordIsrs[i]->GetDocumentCount( );
         }

      saveMatch( );

      while( fb::UniquePtr<IndexInfo> info = mainIsr->NextDocument( ) )
         {
         saveMatch( );
         }
      }

   fb::Vector<fb::SizeT> GetDocFrequencies( )
      {
      return doc_frequencies;
      }

   fb::Vector<rank_stats> GetDocumentsToRank( )
      {
      return documents_to_rank;
      }


private:
   fb::UniquePtr<ISR> mainIsr;
   fb::UniquePtr<DocumentISR> docIsr;
   fb::Vector<fb::UniquePtr<WordISR>> wordIsrs;
   uint32_t page_store_num;
   fb::Vector<fb::SizeT> doc_frequencies;
   fb::Vector<rank_stats> documents_to_rank;

   void saveMatch( ) 
      {
      Location docEnd = docIsr->Seek( mainIsr->GetCurrentInfo( )->GetStartLocation( ) )->GetEndLocation( );
      uint32_t docLength = docIsr->GetDocumentLength( );
      Location docStart = docEnd - docLength;
      fb::Vector<uint32_t> term_freqs( wordIsrs.size( ), 0 );
      for( fb::SizeT i = 0; i < term_freqs.size( ); ++i)
         {
         fb::UniquePtr<IndexInfo> wordInfo = wordIsrs[i]->Seek( docStart );
         while( wordInfo && wordInfo->GetEndLocation( ) < docEnd ) 
            {
            ++term_freqs[i];
            wordInfo = wordIsrs[i]->Next( );
            }
         }

      rank_stats stats;
      stats.page_store_number = page_store_num;
      stats.page_store_index = mainIsr->GetDocumentId( );
      stats.total_term_count = docLength;
      stats.term_freq = std::move( term_freqs );

      documents_to_rank.emplaceBack( rank_stats{ page_store_num, mainIsr->GetDocumentId( ), docLength, std::move( term_freqs ) } );
      }

   };