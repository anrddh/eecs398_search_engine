// Added by Jaeyoon Kim 11/15/2019

#include "url_tcp.hpp"
// The header for endian (for changing endianess for uint64_t)
// might be different for other os
#if defined(OS_MACOSX)
  #include <machine/endian.h>
#elif defined(OS_SOLARIS)
  #include <sys/isa_defs.h>
  #ifdef _LITTLE_ENDIAN
    #define LITTLE_ENDIAN
  #else
    #define BIG_ENDIAN
  #endif
#elif defined(OS_FREEBSD) || defined(OS_OPENBSD) || defined(OS_NETBSD) ||\
      defined(OS_DRAGONFLYBSD)
  #include <sys/types.h>
  #include <sys/endian.h>
#else
  #include <endian.h>
#endif

#include <cassert>

using namespace fb;

void send_int(int sock, uint32_t num) {
   num = htonl(num);
   if (send(sock , &num , sizeof(uint32_t) , 0 ) == -1) {
      throw SocketException("TCP Utility: send_int failed");
   }
}

uint32_t recv_int(int sock) {
   uint32_t num;

   if (recv(sock, &num, sizeof(uint32_t), MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_int failed");
   }
   return ntohl(num);
}

void send_uint64_t(int sock, uint64_t num) {
   num = htobe64(num);
   if (send(sock , &num , sizeof(uint64_t) , 0 ) == -1) {
      throw SocketException("TCP Utility: send_uint64_t failed");
   }
}

uint64_t recv_uint64_t(int sock) {
   uint64_t num;

   if (recv(sock, &num, sizeof(uint64_t), MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_uint64_t failed");
   }
   return be64toh(num);
}

void send_str(int sock, const fb::StringView str) {
   uint32_t size = str.size();
   send_int(sock, size);

   // Should send null character as well
   if (send(sock , str.data() , size + 1 , 0 ) == -1) {
      throw SocketException("TCP Utility: send_str failed");
   }
}

String recv_str(int sock) {
   uint32_t size = recv_int(sock);

   String url;
   url.resize( size ); // resize to length of string (not counting null character)
   
   // Need to write null character as well
   if (recv(sock, url.data(), size + 1, MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_str failed");
   }

   // Make sure the string is null terminiated
   assert( url.data()[size] == '\0' );

   return url;
}
