#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>

#include "isr/isr.hpp"
#include "isr/index_reader.hpp"

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
   fstat(f, &details);

   char * startOfIndex = ( char * ) mmap( nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, file, 0 );

   IndexReader<4> reader(startOfIndex);
   fb::Vector<fb::UniquePtr<ISR>> ISRs;
   for( int i = 2;  i < argc;  ++i )
      {
      fb::String word(argv[i]);
      ISRs.pushBack( reader.OpenWordISR( word ) );
      }

   AndISR andISR( std::move( ISRs ), reader.GetDocumentISR( ) );
   fb::Vector<Location> documents;
   documents.pushBack( andISR.GetCurrentInfo( ).GetStartLocation( ) );

   while(fb::UniquePtr<IndexInfo> info = andISR.next( ))
      {
      documents.pushBack( info.GetStartLocation( ) );
      }

   DocumentISR docISR = reader.GetDocumentISR( );
   for(Location loc : documents)
      fb::UniquePtr<DocumentInfo> docInfo = docISR.Seek( loc );
      std::cout << docInfo.GetDocId( ) << std::endl;
   }