#pragma once

#include "fb/memory.hpp"

#include "isr/isr.hpp"

struct rank_stats
   {
	uint32_t page_store_number;
   uint32_t page_stoe_index;
	unsigned int total_term_count; // doc length
	vector<uint32_t> term_freq;
   };

class ConstraintSolver 
   {
public:
   ConstraintSolver( fb::UniquePtr<ISR> isrIn, fb::UniquePtr<DocumentISR> docIsrIn, const fb::vector<fb::UniquePtr<WordISR>> &wordIsrsIn, uint32_t page_store_number ) 
   : isr( std::move( isrIn ) ), docIsr( std::move( docIsrIn ) ), wordsIsrs( std::move( wordIsrsIn ) ), page_store_num( page_store_number ), doc_frequencies(wordIsrs.size( ), 0)
      { 
      for( fb::SizeT i = 0; i < doc_frequencies.size( ); ++i )
         {
         doc_frequencies[i] = wordsIsrs[i]->GetDocumentCount( );
         }

      saveMatch( );

      while( fb::UniquePtr<IndexInfo> info = andISR.NextDocument( ) )
         {
         saveMatch( ):
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
   fb::UniquePtr<ISR> isr;
   DocumentISR docIsr;
   const fb::Vector<fb::UniquePtr<WordISR>> wordIsrs;
   uint32_t page_store_num;
   fb::Vector<fb::SizeT> doc_frequencies;
   fb::Vector<rank_stats> documents_to_rank;

   void saveMatch( ) 
      {
      Location docEnd = docIsr.Seek( isr->GetCurrentInfo( )->GetStartLocation( ) )->GetEndLocation( );
      uint32_t docLength = docIsr.GetDocumentLength( );
      Location docStart = docEnd - docLength;
      fb::Vector<uint32_t> term_freqs( wordIsrs.size( ), 0 );
      for( fb::SizeT i = 0; i < term_freqs.size( ); ++i)
         {
         fb::UniquePtr<IndexInfo> wordInfo = wordIsrs[0]->Seek( docStart );
         while( wordInfo && wordInfo->GetEndLocation( ) < docEnd ) 
            {
            ++term_freqs[i];
            }
         }

      rank_stats stats;
      stats.page_store_number = page_store_num;
      stats.page_store_index = isr->GetDocumentId( );
      stats.total_term_count = docLength;
      stats.term_freq = std::move( term_freqs );

      documents_to_rank.emplace_back( page_store_num, isr->GetDocumentId, docLength, std::move( term_freqs ) );
      }

   };