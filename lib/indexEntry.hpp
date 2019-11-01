#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>

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

struct headerByte {
   uint8_t hasHeader: 1;
   uint8_t size: 2;
};

static_assert(sizeof(headerByte) == 1);

struct oneByteNoHeader {
   static constexpr int sizeEncoding = 0;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint8_t value: 5; // max value 0x1f
};

static_assert(sizeof(oneByteNoHeader) == 1);

struct twoByteNoHeader {
   static constexpr int sizeEncoding = 1;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint16_t value: 13; // max value 0x1fff
};

static_assert(sizeof(twoByteNoHeader) == 2);

struct fourByteNoHeader {
   static constexpr int sizeEncoding = 2;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint32_t value: 29; // max value 0x1f ff ff ff
};

static_assert(sizeof(fourByteNoHeader) == 4);

struct eightByteNoHeader {
   static constexpr int sizeEncoding = 3;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint64_t value: 61; // max value 0x1f ff ff ff ff ff ff ff
};

static_assert(sizeof(eightByteNoHeader) == 8);

struct twoByteWithHeader {
   static constexpr int sizeEncoding = 1;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint8_t header: 4;
   uint16_t value: 9; // max value 0x01 ff
};

static_assert(sizeof(twoByteWithHeader) == 2);


struct fourByteWithHeader {
   static constexpr int sizeEncoding = 2;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint8_t header: 4;
   uint32_t value: 25; // max value 0x01 ff ff ff
};

static_assert(sizeof(fourByteWithHeader) == 4);

struct eightByteWithHeader {
   static constexpr int sizeEncoding = 3;
   uint8_t hasHeader: 1;
   uint8_t size: 2;
   uint8_t header: 4;
   uint64_t value: 57; // max value 0x01 ff ff ff ff ff ff ff
};

static_assert(sizeof(eightByteWithHeader) == 8);

constexpr uint64_t oneByteNoHeaderMaxVal = 0x1f;
constexpr uint64_t twoByteNoHeaderMaxVal = 0x1fff;
constexpr uint64_t fourByteNoHeaderMaxVal = 0x1fffffff;
constexpr uint64_t eightByteNoHeaderMaxVal = 0x1fffffffffffffff;

constexpr uint64_t twoByteHeaderMaxVal = 0x01ff;
constexpr uint64_t fourByteHeaderMaxVal = 0x01ffffff;
constexpr uint64_t eightByteHeaderMaxVal = 0x01ffffffffffffff;

template <typename castType>
inline char* add_num_no_header(char* curr, size_t num) {
   ((castType*) curr)->hasHeader = 0;
   ((castType*) curr)->size = castType::sizeEncoding;
   ((castType*) curr)->value = num;
   return curr + sizeof(castType);
}

template <typename castType>
inline char* add_num_with_header(char* curr, size_t num, uint8_t header) {
   ((castType*) curr)->hasHeader = 1;
   ((castType*) curr)->size = castType::sizeEncoding;
   ((castType*) curr)->header = header;
   ((castType*) curr)->value = num;
   return curr + sizeof(castType);
}

inline char* add_num(char* curr, size_t num, uint8_t header = 0) {
   if (header) {
      if (num <= twoByteHeaderMaxVal) {
         return add_num_with_header<twoByteWithHeader>(curr, num, header);
      }

      if (num <= fourByteHeaderMaxVal) {
         return add_num_with_header<fourByteWithHeader>(curr, num, header);
      }

      return add_num_with_header<eightByteWithHeader>(curr, num, header);
   } else {
      if (num <= oneByteNoHeaderMaxVal) {
         return add_num_no_header<oneByteNoHeader>(curr, num);
      }

      if (num <= twoByteNoHeaderMaxVal) {
         return add_num_no_header<twoByteNoHeader>(curr, num);
      }

      if (num <= fourByteNoHeaderMaxVal) {
         return add_num_no_header<fourByteNoHeader>(curr, num);
      }

      return add_num_no_header<eightByteNoHeader>(curr, num);
   }
}

template <typename castType>
inline char* read_number_no_header(char* curr, uint64_t &num, uint8_t &header) {
   header = 0;
   num = ((castType*) curr)->value;
   return curr + sizeof(castType);
}

template <typename castType>
inline char* read_number_with_header(char* curr, uint64_t &num, uint8_t &header) {
   header = ((castType*) curr)->header;
   num = ((castType*) curr)->value;
   return curr + sizeof(castType);
}

char* read_number(char* curr, uint64_t &num, uint8_t &header) {
   if (((headerByte*) curr)->hasHeader) {
      switch (((headerByte*) curr)->size) {
         case 1:
            return read_number_with_header<twoByteWithHeader>(curr, num, header);
         case 2:
            return read_number_with_header<fourByteWithHeader>(curr, num, header);
         case 3:
            return read_number_with_header<eightByteWithHeader>(curr, num, header);
         default:
            assert(false);
      } 
   } else {
      switch (((headerByte*) curr)->size) {
         case 0:
            return read_number_no_header<oneByteNoHeader>(curr, num, header);
         case 1:
            return read_number_no_header<twoByteNoHeader>(curr, num, header);
         case 2:
            return read_number_no_header<fourByteNoHeader>(curr, num, header);
         case 3:
            return read_number_no_header<eightByteNoHeader>(curr, num, header);
         default:
            assert(false);
      }
   }
}

}; // Namespace fb
