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
#include "isr/container_isr.hpp"
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

   IndexReader reader(startOfIndex);

   if(argc > 3)
      {
      fb::Vector<fb::UniquePtr<WordISR>> WordIsrs;
      fb::Vector<fb::UniquePtr<ISR> > IncludeWords;
      fb::Vector<fb::UniquePtr<ISR> > ExcludeWords;
      for( int i = 2;  i < argc;  ++i )
         {
         if(argv[i][0] == '!')
            {
            fb::String word(argv[i] + 1);
            std::cout << "!" << word;
            if( reader.WordExists( word ) )
               {
               ExcludeWords.pushBack( reader.OpenWordISR( word ) );
               std::cout << " does exist" << std::endl;
               }
            else
               {
               std::cout << " does not exists" << std::endl;
               }
            }
         else
            {
            fb::String word(argv[i]); 
            std::cout << word;
            if( reader.WordExists( word ) )
               {
               IncludeWords.pushBack( reader.OpenWordISR( word ) );
               WordIsrs.pushBack( reader.OpenWordISR( word ) );
               std::cout << " does exist" << std::endl;
               }
            else
               {
               std::cout << " does not exist" << std::endl;
               }
            }
         }

      if( IncludeWords.empty( ) )
         {
         std::cout << "no words in the query are in the index" << std::endl;
         }
      fb::UniquePtr<AndISR> IncludeIsr = fb::makeUnique<AndISR>( std::move( IncludeWords ), reader.OpenDocumentISR( ) );
      fb::UniquePtr<ISR> Final;

      if( ExcludeWords.empty( ) )
         {
         Final = std::move( IncludeIsr );
         }
      else
         {
         fb::UniquePtr<AndISR> ExcludeIsr = fb::makeUnique<AndISR>( std::move( ExcludeWords ), reader.OpenDocumentISR( ) );
         Final = fb::makeUnique<ContainerISR>( std::move( IncludeIsr ), std::move( ExcludeIsr ), reader.OpenDocumentISR( ) );
         }

      //*
      ConstraintSolver solver( std::move( Final ), reader.OpenDocumentISR( ), std::move( WordIsrs ), 0);
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