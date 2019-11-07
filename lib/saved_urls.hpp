// Created by Jaeyoon Kim 11/6/2019
#pragma once
#include <string> // TODO delete
#include "Exception.hpp"
#include "SavedObject.hpp"

namespace fb {

// This is the class where one can save list of urls
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to. 
class SavedUrls {
public:
   SavedUrls( std::string filename ) : disk_array( filename ) {}

   SizeT add_url( std::string url) {
      // Make sure to increment the cursor before writing so that
      // other threads can work on the array as well
      // We also need to write the null byte
      SizeT str_begin = disk_array.cursor->fetch_add(url.length() + 1); 
      strcpy( disk_array.file_ptr + str_begin, url.c_str() ); 
      return str_begin;
   }

   inline char* get_url(SizeT offset) 
      { 
      return disk_array.file_ptr + offset; 
      }
private:
   SavedObj<char> disk_array;
};

};
