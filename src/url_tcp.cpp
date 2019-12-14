// Added by Jaeyoon Kim 11/15/2019

#include <tcp/url_tcp.hpp>
#include <iostream>

// The header for endian (for changing endianess for uint64_t)
// might be different for other os
#if defined(OS_MACOSX) || defined(__APPLE__)

  #include <libkern/OSByteOrder.h>

  #define htobe16(x) OSSwapHostToBigInt16(x)
  #define htole16(x) OSSwapHostToLittleInt16(x)
  #define be16toh(x) OSSwapBigToHostInt16(x)
  #define le16toh(x) OSSwapLittleToHostInt16(x)

  #define htobe32(x) OSSwapHostToBigInt32(x)
  #define htole32(x) OSSwapHostToLittleInt32(x)
  #define be32toh(x) OSSwapBigToHostInt32(x)
  #define le32toh(x) OSSwapLittleToHostInt32(x)

  #define htobe64(x) OSSwapHostToBigInt64(x)
  #define htole64(x) OSSwapHostToLittleInt64(x)
  #define be64toh(x) OSSwapBigToHostInt64(x)
  #define le64toh(x) OSSwapLittleToHostInt64(x)

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

void send_char(int sock, char c) {
   if (send(sock , &c , sizeof(c) , MSG_NOSIGNAL ) == -1) {
      throw SocketException("TCP Utility: send_char failed");
   }
}

char recv_char(int sock) {
   char c;

   if (recv(sock, &c, sizeof(c), MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_char failed");
   }
   return c;
}

void send_int(int sock, uint32_t num) {
   num = htonl(num);
   if (send(sock , &num , sizeof(uint32_t) , MSG_NOSIGNAL ) == -1) {
      throw SocketException("TCP Utility: send_int failed");
   }
}

uint32_t recv_int(int sock) {
   uint32_t num;

   if (recv(sock, &num, sizeof(uint32_t), MSG_WAITALL) <= 0) {
      perror("error from recv int"); // TODO delete
      throw SocketException("TCP Utility: recv_int failed");
   }
   return ntohl(num);
}

void send_uint64_t(int sock, uint64_t num) {
   num = htobe64(num);
   if (send(sock , &num , sizeof(uint64_t) , MSG_NOSIGNAL ) == -1) {
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
   if (send(sock , str.data() , size + 1 , MSG_NOSIGNAL ) == -1) {
      throw SocketException("TCP Utility: send_str failed");
   }
}

String recv_str(int sock) {
   uint32_t size = recv_int(sock);

   String url;
   url.resize( size ); // resize to length of string (not counting null character)

   // Need to write null character as well
   if (recv(sock, url.data(), size, MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_str failed");
   }

   char c;
   if (recv(sock, &c, sizeof(c), MSG_WAITALL) <= 0) {
      throw SocketException("TCP Utility: recv_str failed");
   }

   // TODO throw!
   // Make sure the string is null terminiated
   if ( c != '\0' ) {
      throw SocketException("TCP Utility: recv_str got non-null terminating string!");
   }

   return url;
}

void send_double(int sock, double d) {
    send_str( sock, toString(d) );
}

double recv_double(int sock) {
    return atof( recv_str( sock ).data() );
}
