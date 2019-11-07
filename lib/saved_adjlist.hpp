// Created by Jaeyoon Kim 11/6/2019
#pragma once
#include "stddef.hpp"
#include "Exception.hpp"
#include "SavedObject.hpp"
#include <string> // TODO delete
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>

namespace fb {

// This is the class where one can save list of adj list
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to. 
class SavedAdjList {
public:
   SavedAdjList( std::string filename ) : disk_array( filename ) {}

   SizeT add_adj_list( std::vector<SizeT> adj_list ) {
      // obtain the old previous value and increment it
      SizeT offset = disk_array.cursor->fetch_add( adj_list.size() ); 
      SizeT* ptr = disk_array.file_ptr + offset;
      for (SizeT i = 0; i < adj_list.size(); ++i) {
         ptr[i] = adj_list[i];
      }
      return offset;
   }

   inline SizeT* get_adj_list(SizeT offset) 
      { 
      return disk_array.file_ptr + offset; 
      }

private:
   SavedObj<SizeT> disk_array;
};
};
