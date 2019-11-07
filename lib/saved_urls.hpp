// Created by Jaeyoon Kim 11/6/2019
#pragma once
#include <atomic>
#include <string>

constexpr MAXFILESIZE = 128e11; // 128 Giga bytes

// This is the class where one can save list of urls
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to. 
class SavedUrls {
public:
   SaveUrls( std::string filename_ ) : filename( filename_ ), cursor( 0 ) {
      int fd = open( filename.c_str(), O_RDWR );
      file_ptr = ( char* ) mmap(nullptr, MAXFILESIZE, PROT_WRITE, MAP_PRIVATE, fd, 0);
   }

   SizeT add_url( std::string url) {
      // obtain the old previous value and increment it
      SizeT str_begin = cursor.fetch_add(url.len() + 1); 
      strcpy( file_ptr + str_begin, url.c_str() ); 
   }

   inline char* get_str(SizeT offset) 
      { 
      return file_ptr + offset; 
      }
private:
   std::string filename;
   std::atomic<SizeT> cursor;
   char* file_ptr;
}
