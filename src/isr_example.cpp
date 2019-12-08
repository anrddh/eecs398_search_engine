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
#include "isr/container_isr.hpp"
#include "isr/constraint_solver.hpp"

int main(int argc, char ** argv )
   {
   if( argc != 2 )
      {
      std::cout << "USAGE: " << argv[0] << " PATH/TO/INDEX" << std::endl;
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
   fb::UniquePtr<WordISR> cat = reader.OpenWordISR( fb::String( "cat" ) );
   fb::UniquePtr<WordISR> dog = reader.OpenWordISR( fb::String( "dog" ) );
   fb::UniquePtr<WordISR> tree = reader.OpenWordISR( fb::String( "tree" ) );

   fb::Vector<fb::UniquePtr<WordISR>> WordsIsrs;
   WordsIsrs.pushBack( reader.OpenWordISR( fb::String( "cat" ) ) );
   WordsIsrs.pushBack( reader.OpenWordISR( fb::String( "dog" ) ) );
   WordsIsrs.pushBack( reader.OpenWordISR( fb::String( "tree" ) ) );

   fb::Vector<fb::UniquePtr<ISR>> catAndDogList;
   catAndDogList.pushBack( std::move( cat ) );
   catAndDogList.pushBack( std::move( dog ) );

   fb::UniquePtr<OrISR> catOrDog = fb::makeUnique<OrISR>( std::move(catAndDogList), reader.OpenDocumentISR( ) );

   fb::Vector<fb::UniquePtr<ISR>> andList;
   andList.pushBack( std::move( catOrDog ) );
   andList.pushBack( std::move( tree ) );
   
   fb::UniquePtr<AndISR> final = fb::makeUnique<AndISR>( std::move(andList), reader.OpenDocumentISR( ) );

   ConstraintSolver solver( std::move( final ), reader.OpenDocumentISR( ), std::move( WordsIsrs ), 0);
   solver.GetDocFrequencies( );
   fb::Vector<rank_stats> rankingData = solver.GetDocumentsToRank( );
   
   for(const rank_stats &stat : rankingData)
      {
      std::cout << stat.page_store_index << std::endl;
      }
   
   }