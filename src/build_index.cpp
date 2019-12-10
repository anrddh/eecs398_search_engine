#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream> 

#include "fb/string.hpp"
#include "index/index_builder.hpp"

int main( int argc, char ** argv )
   {
      if( argc != 4 )
      {
      std::cout << "USAGE: " << argv[0] << " [PATH TO INDEX FOLDER] [PREFIX FILE NAME] [NUM_PAGE_STORE_FILES]" << std::endl;
      exit( 1 );
      }

   fb::String path( argv[1] );
   if(path.back( ) != '/')
      {
      path.pushBack('/');
      }
   fb::String PageStorePrefix(argv[2]);
   int num_page_store_files = atoi(argv[3]);

   for(int i = 0; i < num_page_store_files; ++i)
      {
      IndexBuilder builder(path);
      fb::String PageStoreFileName = (path + PageStorePrefix + fb::toString(i));
      std::cout << PageStoreFileName << std::endl;
      int PageStoreFile = open(PageStoreFileName.data(), O_RDWR);
      if(PageStoreFile < 0)
         {
         std::cout << "ERROR: PageStore" << i << " not found" << std::endl;
         }
      else
         {
         struct stat details;
         fstat(PageStoreFile, &details);
         uint64_t * start = (uint64_t *) mmap(nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, PageStoreFile, 0);

         builder.build_chunk(start, i);
         }
      }
   }