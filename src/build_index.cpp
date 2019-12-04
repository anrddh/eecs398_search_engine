#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream> 

#include "fb/string.hpp"
#include "index/index_builder.hpp"

int main(int argc, char ** argv)
   {
   if(argc != 3) 
      {
         fb::String ErrorMessage = fb::String("Usage: ") + fb::String(argv[0]) + fb::String(" path number");
      }
   fb::String path(argv[1]);
   fb::String Number(argv[2]);
   int f = open((path + "PageStore" + Number).data(), O_RDWR);
   if(f < 0)
      {
      // write debug message
      exit(1);
      }
   struct stat details;
   fstat(f, &details);

   uint64_t * start = (uint64_t *) mmap(nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, f, 0);
   IndexBuilder<8> builder(path);
   builder.build_chunk(start, fb::stoi(Number));
   std::cout << "DONE" << std::endl;
   }