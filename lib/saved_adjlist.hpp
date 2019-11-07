// Created by Jaeyoon Kim 11/6/2019
#pragma once
#include "string.hpp"
#include "Exception.hpp"
#include "vector.hpp"
#include <atomic>
#include <fcntl.h>
#include <sys/mman.h>

using namespace fb {

constexpr MAXFILESIZE = 128e11; // 128 Giga bytes


// This is the class where one can save list of adj list
// ASSUMES that there won't be more than 128 Gb of urls
// Note that each file will be mapping 128 Gb of virtual address
// However, the pages will not be allocated for the files until
// they are written to.
class SavedAdjList {
public:
   SavedAdjList( String filename )  {
      int fd = open( filename.c_str(), O_RDWR );
      struct stat sb;
      if ( fstat(fd, &sb) == -1 )
      {
         throw Exception( (String("SavedAdjList: Failed to open file") + filename).c_str() );
      }

      // TODO check for off by one error
      cursor = sb.st_size / sizeof(SizeT);
      if ( sb.st_size % sizeof(SizeT) == 0 ) {
         throw Exception( (String("SavedAdjList: File has invalid size") + filename).c_str() );
      }

      file_ptr = ( SizeT* ) mmap(nullptr, MAXFILESIZE, PROT_WRITE, MAP_PRIVATE, fd, 0);
   }

   SizeT add_adj_list( Vector<SizeT> adj_list ) {
      // obtain the old previous value and increment it
      SizeT offset = cursor.fetch_add( adj_list.size() );
      SizeT* ptr = file_ptr + offset;
      for (SizeT i = 0; i < adj_list.size(); ++i) {
         ptr[i] = adj_list[i];
      }
      return offset;
   }

   inline char* get_adj_list(SizeT offset)
      {
      return file_ptr + offset;
      }
private:
   std::atomic<SizeT> cursor;
   SizeT* file_ptr;
}
};
