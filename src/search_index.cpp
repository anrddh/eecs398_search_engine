#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <iostream>

#include "isr/index_reader.hpp"

#include "isr/word_isr.hpp"
#include "isr/document_isr.hpp"
#include "isr/and_isr.hpp"
#include "isr/or_isr.hpp"
#include "isr/phrase_isr.hpp"
#include "isr/constraint_solver.hpp"

int main(int argc, char ** argv )
   {
   if(argc < 3)
      {
      std::cout << "Usage: " << argv[0] << " index_file word_one [ other_words ]" << std::endl;
      exit(1);
      }

   fb::String index_file(argv[1]);
   int file = open(index_file.data(), O_RDWR);
   if(file < 0)
      {
      // write debug message
      std::cout << "File \'" << index_file << "\' could not be opened" << std::endl;
      exit(1);
      }

   struct stat details;
   fstat(file, &details);

   const char * startOfIndex = ( const char * ) mmap( nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, file, 0 );

   IndexReader<8> reader(startOfIndex);

   if(argc > 3)
      {
      fb::Vector<fb::UniquePtr<WordISR>> ISRs;
      fb::Vector<fb::UniquePtr<WordISR> > wordIsrs;
      for( int i = 2;  i < argc;  ++i )
         {
         fb::String word(argv[i]);
         ISRs.pushBack( reader.OpenWordISR( word ) );
         wordIsrs.pushBack( reader.OpenWordISR( word ) );
         }

      fb::UniquePtr<PhraseISR> phraseISR = fb::makeUnique<PhraseISR>( std::move( ISRs ), reader.OpenDocumentISR( ) );
      //*
      ConstraintSolver solver( std::move( phraseISR ), reader.OpenDocumentISR( ), std::move( wordIsrs ), 0);
      solver.GetDocFrequencies( );
      fb::Vector<rank_stats> rankingData = solver.GetDocumentsToRank( );
      
      for(const rank_stats &stat : rankingData)
         {
         std::cout << stat.page_store_index << std::endl;
         }



      /*/
      std::cout << andISR->GetDocumentId( ) << std::endl;

      while(fb::UniquePtr<IndexInfo> info = andISR->NextDocument( ))
         {
         std::cout << andISR->GetDocumentId( ) << std::endl;
         }
      //*/
      }
   else
      {
      fb::UniquePtr<WordISR> wordISR = reader.OpenWordISR( fb::String( argv[2] ) );
      if(!wordISR)
         {
         std::cout << "Word Not Found!" << std::endl;
         return 0;
         }
      fb::UniquePtr<DocumentISR> docISR = reader.OpenDocumentISR( );
      fb::UniquePtr<IndexInfo> info = wordISR->GetCurrentInfo( );
      while( info )
         {
         docISR->Seek( info->GetEndLocation( ) );
         unsigned int docId = docISR->GetDocumentId( );
         std::cout << docId << std::endl;
         info = wordISR->NextDocument( );
         }
      }
   }