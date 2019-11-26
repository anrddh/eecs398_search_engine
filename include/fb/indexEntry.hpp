// By Jaeyoon Kim

#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>
#include "indexEntryImpl.hpp" // where implementation details are hidden


namespace fb {
// Bit layout - first two bits are dedicated for indicating the number of bytes
// length = 00 -> 1 bytes
// length = 01 -> 2 bytes
// length = 10 -> 4 bytes

/* GIVEN a pointer, will insert a numerical value
 * RETURNs the pointer to end of the inserted data 
 * (first byte that hasn't bit written to yet)
 * example use case
 * char* mem= (char*) new uint64_t[16];
 * mem = add_num(mem, 3);
 * mem = add_num(mem, 1623);
 * mem = add_num(mem, 610516583);
 */
inline char* add_word_post( char* curr, size_t num) 
   {
      if ( num <= fbImpl::oneBytePostMaxVal ) 
         {
         return fbImpl::write_num<fbImpl::oneBytePost>( curr, num );
         }
      if ( num <= fbImpl::twoBytePostMaxVal ) 
         {
         return fbImpl::write_num<fbImpl::twoBytePost>( curr, num );
         }

      return fbImpl::write_num<fbImpl::fourBytePost>( curr, num );
   }

inline char* add_word_sentinel( char* curr )
   {
   return add_word_post(curr, 0);
   }

/* Given a pointer, will copy the stored value to uint64_t &num
 * RETURNs pointer to next value to read
 * example code
 * uint64_t value;
 * mem = read_number(mem, value );
 * cout << value << endl;
 * mem = read_number(mem, value);
 * cout << value << endl;
 * mem = read_number(mem, value);
 * cout << value << endl;
 */
inline char* read_word_post( char* curr, uint64_t &num) 
   {
      switch ( ( ( fbImpl::oneBytePost * ) curr )->size ) 
         {
         case 0:
            return fbImpl::read_number<fbImpl::oneBytePost>( curr, num );
         case 1:
            return fbImpl::read_number<fbImpl::twoBytePost>( curr, num );
         case 2:
            return fbImpl::read_number<fbImpl::fourBytePost>( curr, num );
         default:
            assert( false );
         } 
   }

inline bool is_word_sentinel( char* curr )
   {
   uint64_t num;
   read_word_post(curr, num);
   return num == 0;
   }

// Adds document post. Returns the pointer to next address we should add to
inline char* add_document_post( char* curr, size_t delta, uint32_t url_loc) {
   (* (uint32_t *) curr) = delta;
   curr += sizeof(delta);
   (* (uint32_t *) curr) = url_loc;
   return curr + sizeof(url_loc);
}

inline char* add_document_sentinel( char* curr )
   {
   return add_document_post(curr, 0, 0);
   }

// Reads document post. Returns the pointer to next address we should read from
inline char* read_document_post( char* curr, size_t& delta, uint32_t& url_loc ) {
   delta = (* (uint32_t *) curr);
   curr += sizeof(delta);
   url_loc = (* (uint32_t *) curr);
   return curr + sizeof(url_loc);
}

inline bool is_document_sentinel( char * curr )
   {
   size_t delta;
   uint32_t url_loc;
   read_document_post(curr, delta, url_loc);
   return delta == 0;
   }

}; // Namespace fb
