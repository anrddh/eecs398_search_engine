#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream> 

#include "fb/string.hpp"
#include "index/index_builder.hpp"

int main( int argc, char ** argv )
   {
   if( argc != 2 )
      {
      std::cout << "USAGE: " << argv[0] << " [PATH TO INDEX FOLDER]" << std::endl;
      exit( 1 );
      }

   fb::String path( argv[1] );

   bool initIndexCounter = false;
   int indexCounterFile = open((path + "IndexCounter").data(), O_RDWR);
   if(indexCounterFile < 0)
      {
      indexCounterFile = open((path + "IndexCounter").data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
      initIndexCounter = true;

      if(indexCounterFile < 0)
         {
         std::cout << "ERROR: No IndexCounter file was found and could not create one" << std::endl; 
         exit(1);
         }
      } 
 
   ftruncate(indexCounterFile, sizeof(int));
   int &indexCounter = *(int *) mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, indexCounterFile, 0);
   if( initIndexCounter )
      {
      indexCounter = 0;
      }
/*
   fb::String PageStoreCounterFileName = fb::String(path.begin(), path.end() - 1) + PageStoreCounterFile;
   int PageStoreCounterFile = open(PageStoreCounterFileName.data( ), O_RDWR);
   if( !PageStoreCountereFile )
      {
      std::cout << "ERROR: No PageStoreCounter file was found" << std::endl;
      exit(1);
      }
*/
   int end = 5;
   for(int i = indexCounter; i < end; ++i)
      {
      IndexBuilder builder(path);
      fb::String PageStoreFileName = (path + "PageStore" + fb::toString(i));
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
      ++indexCounter; 
      }
   }