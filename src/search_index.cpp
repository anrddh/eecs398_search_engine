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

   IndexReader<4> reader(startOfIndex);
   /*
   fb::Vector<fb::UniquePtr<ISR>> ISRs;
   for( int i = 2;  i < argc;  ++i )
      {
      fb::String word(argv[i]);
      ISRs.pushBack( reader.OpenWordISR( word ) );
      }

   AndISR andISR( std::move( ISRs ), reader.OpenDocumentISR( ) );
   fb::Vector<Location> documents;
   documents.pushBack( andISR.GetCurrentInfo( )->GetStartLocation( ) );

   while(fb::UniquePtr<IndexInfo> info = andISR.Next( ))
      {
      documents.pushBack( info->GetStartLocation( ) );
      }

   fb::UniquePtr<DocumentISR> docISR = reader.OpenDocumentISR( );
   for(Location loc : documents)
      {
      docISR->Seek( loc );
      std::cout << docISR->GetDocumentId( ) << std::endl;
      }
   }
   */

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