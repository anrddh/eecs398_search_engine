#pragma once

#include <fb/memory.hpp>

#include <isr/isr.hpp>
#include <isr/document_isr.hpp>
#include <disk/constants.hpp>

#include <ranker/ranker.hpp>

#include <query/query_result.hpp>

#include <cmath>
#include <algorithm>

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

   void solve( fb::String dirname )
      {
      for( fb::SizeT i = 0; i < doc_frequencies.size( ); ++i )
         {
         doc_frequencies[i] = log2(TOTAL_DOCUMENTS/double(wordIsrs[i]->GetDocumentCount( )));
         }

      fb::UniquePtr<IndexInfo> info = mainIsr->GetCurrentInfo( );

      while( info )
         {
         saveMatch( dirname );
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

   // fb::Vector<fb::SizeT> GetDocFrequencies( )
   //    {
   //    return std::move(doc_frequencies);
   //    }

   // fb::Vector<rank_stats> GetDocumentsToRank( )
   //    {
   //    return std::move(documents_to_rank);
   //    }


   fb::Vector<QueryResult> results;

private:
   fb::UniquePtr<ISR> mainIsr;
   fb::UniquePtr<DocumentISR> docIsr;
   fb::Vector<fb::UniquePtr<WordISR>> wordIsrs;
   uint32_t page_store_num;
   // fb::Vector<fb::SizeT> doc_frequencies;
   static constexpr double TOTAL_DOCUMENTS = 10000;
   static constexpr fb::SizeT MAX_SNIP_WINDOW = 100;

   fb::Vector<double> doc_frequencies;
   fb::Vector<rank_stats> documents_to_rank;

   void saveMatch( fb::String dirname )
      {
      Location docEnd = docIsr->Seek( mainIsr->GetCurrentInfo( )->GetStartLocation( ) )->GetEndLocation( );
      uint32_t docLength = docIsr->GetDocumentLength( );
      Location docStart = docEnd - docLength;
      fb::Vector<fb::SizeT> occurrences;
      fb::Vector<double> occurencesPerWordIsrs(wordIsrs.size(), 0);

      for(fb::SizeT i = 0; i < occurencesPerWordIsrs.size( ); ++i)
         {
         fb::UniquePtr<IndexInfo> wordInfo = wordIsrs[i]->Seek( docStart );
         while( wordInfo && wordInfo->GetEndLocation( ) < docEnd )
            {
            ++occurencesPerWordIsrs[i];
            occurrences.pushBack(wordInfo->GetStartLocation( ) - docStart );
            wordInfo = wordIsrs[i]->Next( );
            }
         }

      // tfidf
      double current_rank = 0;
      for(size_t i = 0; i < occurencesPerWordIsrs.size(); ++i){
         current_rank += (occurencesPerWordIsrs[i] / double(docLength)) * doc_frequencies[i];
      }

      // sort for now
      if(wordIsrs.size() > 1)
         std::sort(occurrences.begin(), occurrences.end());

      snip_window window = snippet_window_rank(occurrences, docLength, MAX_SNIP_WINDOW);
      SnippetStats stats = { dirname + fb::String(PageStoreFile.data()) + fb::toString((int)page_store_num), mainIsr->GetDocumentId( ), window };

      fb::SizeT doc_UrlId;
      fb::Pair<fb::String, fb::String> SnipTit = GenerateSnippetsAndTitle(stats, doc_UrlId);
      // QueryResult result = { doc_UrlId, SnipTit.second, SnipTit.first, current_rank };
      // Results.add(std::move(result));
      results.emplaceBack(doc_UrlId, SnipTit.second, SnipTit.first, current_rank);

      // documents_to_rank.emplaceBack( rank_stats{ page_store_num, mainIsr->GetDocumentId( ), docLength, 0, std::move( occurrences ) } );
      }

   };
