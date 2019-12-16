#pragma once

#include <fb/memory.hpp>

#include <isr/isr.hpp>
#include <isr/document_isr.hpp>

struct rank_stats
   {
   uint32_t page_store_number;
   uint32_t page_store_index;
   unsigned int total_term_count; // doc length
   fb::SizeT UrlId; //to be set during snippets
   fb::Vector<fb::Vector<uint32_t>> occurrences;
   double rank = -1;
   };

class ConstraintSolver
   {
public:
   ConstraintSolver(fb::UniquePtr<ISR> isrIn,
                    fb::UniquePtr<DocumentISR> docIsrIn,
                    fb::Vector<fb::UniquePtr<WordISR>> wordIsrsIn,
                    uint32_t page_store_number)
   : mainIsr( std::move( isrIn ) ),
     docIsr( std::move( docIsrIn ) ),
     wordIsrs( std::move( wordIsrsIn ) ),
     page_store_num( page_store_number ),
     doc_frequencies(wordIsrs.size( ), 0) { }

   void solve( )
      {
      for( fb::SizeT i = 0; i < doc_frequencies.size( ); ++i )
         {
         doc_frequencies[i] = wordIsrs[i]->GetDocumentCount( );
         }

      fb::UniquePtr<IndexInfo> info = mainIsr->GetCurrentInfo( );

      while( info )
         {
         saveMatch( );
         info = mainIsr->NextDocument( );
         }
      }

   fb::Vector<fb::String> GetWords( )
      {
      fb::Vector<fb::String> words;
      for(fb::UniquePtr<WordISR> &wordIsr : wordIsrs)
         {
         words.pushBack(wordIsr->GetWord( ) );
         }

      return words;
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
      fb::Vector<fb::Vector<uint32_t>> occurrences( wordIsrs.size( ) );
      for( fb::SizeT i = 0; i < occurrences.size( ); ++i)
         {
         fb::UniquePtr<IndexInfo> wordInfo = wordIsrs[i]->Seek( docStart );
         while( wordInfo && wordInfo->GetEndLocation( ) < docEnd )
            {
            occurrences[i].pushBack(wordInfo->GetStartLocation( ) - docStart );
            wordInfo = wordIsrs[i]->Next( );
            }
         }

      documents_to_rank.emplaceBack( rank_stats{ page_store_num, mainIsr->GetDocumentId( ), docLength, 0, std::move( occurrences ) } );
      }

   };
