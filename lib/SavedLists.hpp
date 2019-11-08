// Created by Jaeyoon Kim 11/6/2019
// Generalized into templates by Jaeyoon Kim 11/7/2019
#pragma once
#include "stddef.hpp"
#include "Exception.hpp"
#include "SavedObject.hpp"
#include "vector.hpp"
#include <string> // TODO delete
#include <unistd.h>
#include <sys/stat.h>
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>

namespace fb {

// This is the class where one can save a vector into a contiguous region of memory
// (It uses copy constructor thus it works best with small basic types)
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
template <typename T>
class SavedLists {
public:
   SavedLists( std::string filename ) : disk_array( filename ) {}

   SizeT add_list( const Vector<T>& adj_list ) {
      // obtain the old previous value and increment it
      SizeT offset = disk_array.cursor->fetch_add( adj_list.size() ); 
      SizeT* ptr = disk_array.file_ptr + offset;
      for (SizeT i = 0; i < adj_list.size(); ++i) {
         ptr[i] = adj_list[i];
      }
      return offset;
   }

   inline T* get_list(SizeT offset) 
      { 
      return disk_array.file_ptr + offset; 
      }

private:
   SavedObj<T> disk_array;
};
};
