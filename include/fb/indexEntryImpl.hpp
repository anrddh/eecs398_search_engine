// By Jaeyoon Kim
#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>


namespace fb {
namespace fbImpl {

// bit layout (first byte)
// length = 00 -> 1 bytes
// length = 01 -> 2 bytes
// length = 10 -> 4 bytes

// Rest of the bits are for numerical values


struct oneBytePost 
   {
   static constexpr int sizeEncoding = 0;
   uint8_t size: 2;
   uint8_t value: 6; // max value 0x1f
   };

static_assert( sizeof( oneBytePost ) == 1 );

struct twoBytePost 
   {
   static constexpr int sizeEncoding = 1;
   uint8_t size: 2;
   uint16_t value: 14; // max value 0x1fff
   };

static_assert( sizeof( twoBytePost ) == 2 );

struct fourBytePost 
   {
   static constexpr int sizeEncoding = 2;
   uint8_t size: 2;
   uint32_t value: 30; // max value 0x1f ff ff ff
   };

static_assert( sizeof( fourBytePost ) == 4 );

constexpr uint64_t oneBytePostMaxVal = 0x3f;
constexpr uint64_t twoBytePostMaxVal = 0x3fff;
constexpr uint64_t fourBytePostMaxVal = 0x3fffffff;

template <typename castType>
inline char* write_num( char* curr, uint32_t num ) 
   {
   ( ( castType* ) curr )->size = castType::sizeEncoding;
   ( ( castType* ) curr )->value = num;
   return curr + sizeof( castType );
   }

template < typename castType >
inline char* read_number( char* curr, uint32_t &num ) 
   {
   num = ( ( castType* ) curr )->value;
   return curr + sizeof( castType );
   }

}; // Namespace fbImpl
}; // Namespace fb
