#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fb/string.hpp"
#include "index/index_builder.hpp"

constexpr fb::String ErrorMessage("Usage: ")

int main(int argc, char ** argv)
   {
   if(argc != 3) 
      {
         fb::String ErrorMessage = fb::String("Usage: ") + fb::String(argv[0]) + fb::String(" path number")
      }
   fb::String path(argv[1]);
   fb::String Number(argv[2]);
   int f = open((path + "page_store" + Number).data(), O_RDWR);
   struct stat details;
   fstat(f, &details);

   unsigned int * start = (unsigned int *) mmap(nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, f, 0);
   IndexBuilder<4> builder();
   }