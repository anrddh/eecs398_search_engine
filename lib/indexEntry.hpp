#pragma once

namespace fb {
// bit layout (first byte)
// has header | length | length | bold | italics | header | anchor | extrabit
// length = 00 -> 1 bytes
// length = 01 -> 2 bytes
// length = 10 -> 4 bytes
// length = 11 -> 8 bytes

struct IndexEntry {
   constexpr uint8_t length_bits = 0b01100000;
   constexpr uint8_t has_header_bits = 0b10000000;
   constexpr uint8_t header_bits = 0b00011110;

   constexpr uint8_t 1_byte_no_header_bits = 0x1f; // 5 bits
   constexpr uint16_t 2_byte_no_header_bits = 0x1fff; // 13 bits
   constexpr uint32_t 2_byte_no_header_bits = 0x1fffffff; // 29 bits
   constexpr uint64_t 2_byte_no_header_bits = 0x1fffffffffffffff; // 61 bits

   constexpr uint8_t 1_byte_yes_header_bits = 0x01; // 1 bit
   constexpr uint16_t 2_byte_yes_header_bits = 0x01ff; // 9 bits
   constexpr uint32_t 2_byte_yes_header_bits = 0x01ffffff; // 25 bits
   constexpr uint64_t 2_byte_yes_header_bits = 0x01ffffffffffffff; // 57 bits

   void get_delta(uint8_t& num_bytes, size_t& delta) {
      num_bytes = *((uint8_t *) this) & length_bits;
      num_bytes = num_bytes >>> 5;
      if ( *((uint8_t *) this) & has_header_bits) {
         switch (num_bytes) {
            case 0:
               num_bytes = 1;
               delta = *((uint8_t *) this) & 1_byte_no_header_bits;
               return;
            case 1:
               num_bytes = 2;
               delta = *((uint16_t *) this) & 2_byte_no_header_bits;
               return;
            case 2:
               num_bytes = 4;
               delta = *((uint32_t *) this) & 4_byte_no_header_bits;
               return;
            case 3:
               num_bytes = 8;
               delta = *((uint64_t *) this) & 8_byte_no_header_bits;
               return;
            defualt:
               assert(false);
         }
      } else {
         switch (num_bytes) {
            case 0:
               num_bytes = 1;
               delta = *((uint8_t *) this) & 1_byte_yes_header_bits;
               return;
            case 1:
               num_bytes = 2;
               delta = *((uint16_t *) this) & 2_byte_yes_header_bits;
               return;
            case 2:
               num_bytes = 4;
               delta = *((uint32_t *) this) & 4_byte_yes_header_bits;
               return;
            case 3:
               num_bytes = 8;
               delta = *((uint64_t *) this) & 8_byte_yes_header_bits;
               return;
            defualt:
               assert(false);
         }
      }
   }

   constexpr uint16_t 2byte_size_header = 0x2000;
   constexpr uint32_t 4byte_size_header = 0x40000000;
   constexpr uint32_t 8byte_size_header = 0x6000000000000000;

   constexpr uint8_t bold_flag = 0b00010000;
   constexpr uint8_t italics_flag = 0b00001000;
   constexpr uint8_t headers_flag = 0b00000100;
   constexpr uint8_t anchors_flag = 0b00000010;

   // To modify header, do the following
   // ex to specify bold_flag and italics flag
   // add_value( some number, bold_flag | italics);
   void add_value(size_t value, uint8_t header = 0) {
      if ( !header ) {
         if ( value <= 1_byte_no_header_bits ) {
            *((uint8_t*) this) = value;
            return;
         }
         if ( value <= 2_byte_no_header_bits ) {
            *((uint16_t*) this) = value & 2byte_size_header;
            return;
         }
         if ( value <= 4_byte_no_header_bits ) {
            *((uint32_t*) this) = value & 4byte_size_header;
            return;
         }

         *((uint64_t*) this) = value & 8byte_size_header;
         return;
      } else {
         *((uint8_t*) this) = *((uint8_t*) this) & header;
         if ( value <= 1_byte_yes_header_bits ) {
            *((uint8_t*) this) = value;
            return;
         }
         if ( value <= 2_byte_yes_header_bits ) {
            *((uint16_t*) this) = value & 2byte_size_header;
            return;
         }
         if ( value <= 4_byte_yes_header_bits ) {
            *((uint32_t*) this) = value & 4byte_size_header;
            return;
         }

         *((uint64_t*) this) = value & 8byte_size_header;
         return;
      }
   }
};
}

}; // Namespace fb
