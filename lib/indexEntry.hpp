#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>
#include "indexEntryImpl.hpp" // where implementation details are hidden

namespace fb {
// bit layout (first byte)
// has header | length | length | bold | italics | header | anchor | extrabit
// length = 00 -> 1 bytes
// length = 01 -> 2 bytes
// length = 10 -> 4 bytes
// length = 11 -> 8 bytes

uint8_t BOLD_FLAG = 0b1000;
uint8_t ITALICS_FLAG = 0b0100;
uint8_t HEADER_FLAG = 0b0010;
uint8_t ANCHOR_FLAG = 0b0001;

/* GIVEN a pointer and (optional header), will insert a numerical value with associated header
 * RETURNs the pointer to end of the inserted data 
 * (first byte that hasn't bit written to yet)
 * example use case
 * char* mem= (char*) new uint64_t[16];
 * mem = add_num(mem, 3, BOLD_FLAG | ITALICS_FLAG);
 * mem = add_num(mem, 1623, ITALICS_FLAG);
 * mem = add_num(mem, 610516583293);
 */
inline char* add_num( char* curr, size_t num, uint8_t header = 0 ) 
   {
   if (header) 
      {
      if ( num <= fbImpl::twoByteHeaderMaxVal ) 
         {
         return fbImpl::add_num_with_header<fbImpl::twoByteWithHeader>( curr, num, header );
         }

      if ( num <= fbImpl::fourByteHeaderMaxVal ) 
         {
         return fbImpl::add_num_with_header<fbImpl::fourByteWithHeader>( curr, num, header );
         }

      return fbImpl::add_num_with_header<fbImpl::eightByteWithHeader>( curr, num, header );
      } 
   else 
      {
      if ( num <= fbImpl::oneByteNoHeaderMaxVal ) 
         {
         return fbImpl::add_num_no_header<fbImpl::oneByteNoHeader>( curr, num );
         }

      if ( num <= fbImpl::twoByteNoHeaderMaxVal ) 
         {
         return fbImpl::add_num_no_header<fbImpl::twoByteNoHeader>( curr, num );
         }

      if ( num <= fbImpl::fourByteNoHeaderMaxVal ) 
         {
         return fbImpl::add_num_no_header<fbImpl::fourByteNoHeader>( curr, num );
         }

      return fbImpl::add_num_no_header<fbImpl::eightByteNoHeader>( curr, num );
      }
   }

/* Given a pointer, will read the stored num and the header.
 * RETURNs pointer to next value to read
 * example code
 * uint64_t value;
 * uint8_t header;
 * mem = read_number(mem, value, header);
 * cout << int(header) << " " << value << endl;
 * mem = read_number(mem, value, header);
 * cout << int(header) << " " << value << endl;
 * mem = read_number(mem, value, header);
 * cout << int(header) << " " << value << endl;
 */
inline char* read_number( char* curr, uint64_t &num, uint8_t &header ) 
   {
   if ( ( ( fbImpl::headerByte* ) curr )->hasHeader ) 
      {
      switch ( ( ( fbImpl::headerByte* ) curr )->size ) 
         {
         case 1:
            return fbImpl::read_number_with_header<fbImpl::twoByteWithHeader>( curr, num, header );
         case 2:
            return fbImpl::read_number_with_header<fbImpl::fourByteWithHeader>( curr, num, header );
         case 3:
            return fbImpl::read_number_with_header<fbImpl::eightByteWithHeader>( curr, num, header );
         default:
            assert( false );
         } 
      } 
   else 
      {
      switch ( ( ( fbImpl::headerByte* ) curr )->size ) 
         {
         case 0:
            return fbImpl::read_number_no_header<fbImpl::oneByteNoHeader>( curr, num, header );
         case 1:
            return fbImpl::read_number_no_header<fbImpl::twoByteNoHeader>( curr, num, header );
         case 2:
            return fbImpl::read_number_no_header<fbImpl::fourByteNoHeader>( curr, num, header );
         case 3:
            return fbImpl::read_number_no_header<fbImpl::eightByteNoHeader>( curr, num, header );
         default:
            assert( false );
         }
      }
   }

}; // Namespace fb
