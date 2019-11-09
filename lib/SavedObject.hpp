// Created by Jaeyoon Kim 11/7/2019
#pragma once
#include "stddef.hpp"
#include "Exception.hpp"
#include "string.hpp"
#include <iostream> // TODO delete
#include <unistd.h>
#include <sys/stat.h>
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>

namespace fb {

constexpr SizeT MAXFILESIZE = 0x1000000000; // 128 Giga bytes
//constexpr SizeT MAXFILESIZE = 1000; // 128 Giga bytes


// This is the class that represents an array saved on disk
// ASSUMES that there won't be more than 128 Gb of data
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
template <typename T>
class SavedObj {
public:
   SavedObj( std::string filename )  {
      fd = open( filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH );
      if ( fd == -1 )
      {
         throw Exception( (std::string("SavedAdjList: Failed to open file ") + filename).c_str() );
      }
      ftruncate(fd, MAXFILESIZE);

      cursor = (std::atomic<SizeT>* )  mmap(nullptr, MAXFILESIZE, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
      file_ptr = reinterpret_cast<T*> (cursor + 1);
   }

   ~SavedObj() {
      close(fd);
   }

   std::atomic<SizeT>* cursor;
   T* file_ptr;

private:
   int fd;
};
}; //fb
