// Created by Jaeyoon Kim 11/6/2019
#pragma once
#include "string.hpp"
#include "Exception.hpp"
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>

using namespace fb {

constexpr MAXFILESIZE = 128e11; // 128 Giga bytes

// This is the class where one can save list of urls
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to. 
class SavedUrls {
public:
   SaveUrls( String filename )  {
      int fd = open( filename.c_str(), O_RDWR );
      struct stat sb;
      if ( fstat(fd, &sb) == -1 ) 
      {
         throw Exception( (String("Failed to open file ") + filename).c_str() );
      }

      // TODO check for off by one error
      // Because the null character doesn't count for file length (check if this is true)
      // we need to go one past the end to write
      cursor = sb.st_size + 1; 

      file_ptr = ( char* ) mmap(nullptr, MAXFILESIZE, PROT_WRITE, MAP_PRIVATE, fd, 0);
   }

   SizeT add_url( std::string url) {
      // obtain the old previous value and increment it
      SizeT str_begin = cursor.fetch_add(url.len() + 1); 
      strcpy( file_ptr + str_begin, url.c_str() ); 
      return str_begin;
   }

   inline char* get_str(SizeT offset) 
      { 
      return file_ptr + offset; 
      }
private:
   std::atomic<SizeT> cursor;
   char* file_ptr;
}
};
