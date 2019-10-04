#pragma once

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <openssl/ssl.h>
#include <array>

#include <iostream>
#include <cassert>
#include <string>

namespace fb
{

//void PrintAddress( const sockaddr_in *s, const size_t saLength )
   //{
   //const struct in_addr *ip = &s->sin_addr;
   //uint32_t a = ntohl( ip->s_addr );

   //cout << "Host address length = " << saLength << " bytes" << endl;
   //cout << "Family = " << s->sin_family <<
   //", port = " << ntohs( s->sin_port ) <<
   //", address = " << ( a >> 24 ) << '.' <<
      //( ( a >> 16 ) & 0xff ) << '.' <<
      //( ( a >> 8 ) & 0xff ) << '.' <<
      //( a & 0xff ) << endl;
   //}

struct View {
    const char *p = nullptr;
    std::size_t s = 0;
};

std::ostream& operator<<(std::ostream &os, View &v) {
    for (size_t i = 0; i < v.s; ++i) {
        os << v.p[i];
    }

    return os;
}

class ParsedUrl
   {
   public:
      const std::string url;

      View Service, Host, Port, Path;

      ParsedUrl( std::string url_ ) : url(std::move(url_))
         {
         Service.p = url.c_str();
         const char Colon = ':', Slash = '/';

         std::size_t p = url.find(':');
         if (p == std::string::npos) {
             Path.p = url.c_str();
             Path.s = url.size();
            return;
         }

         Service.s = p++;

         if ( url[p] == Slash )
             p++;
         if ( url[p] == Slash )
             p++;

         std::cout << "got here with p: " << p << '\n';

         Host.p = url.c_str() + p;

         // check for stl algorithms
         for ( ; p < url.size() && url[p] != Slash && url[p] != Colon; ++p )
             ;

         Host.s = url.c_str() + p - Host.p;

         if (p < url.size() && url[p] == Colon) {
           // Port specified.  Skip over the colon and
           // the port number.
             ++p;
             Port.p = url.c_str() + p;
             for (; p < url.size() && url[p] != Slash; ++p)
                 ;
             Port.s = url.c_str() + p - Port.p;
         }

         Path.p = url.c_str() + p;
         Path.s = url.size() - p;
      }

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
      constexpr static std::array<const char *, 2> allowed_services = {"https", "http"};
   };

   std::ostream& operator << ( std::ostream& os, ParsedUrl &testguy )
      {
      os << "complete: " << testguy.url << std::endl
         << "service: " << testguy.Service << std::endl
         << "host: " << testguy.Host << std::endl
         << "port: " << testguy.Port << std::endl
         << "path: " << testguy.Path << std::endl;
      return os;
      }


    //std::string GetHTTPS( std::string in_url )
    //   {
    //
    //   //if ( argc != 2 )
    //      //{
    //      //std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
    //      //return 1;
    //      //}
    //
    //   // Parse the URL
    //   ParsedUrl url( in_url.c_str( ) );
    //
    //   //test stuff
    //   //cout << "Service: " << url.Service
    //         //<< ", Host: " << url.Host
    //         //<< ", Port" << url.Port
    //         //<< ", Path" << url.Path << endl;
    //
    //   // Get the host address.
    //   struct addrinfo *address, hints;
    //   memset( &hints, 0, sizeof( hints) );
    //   hints.ai_family = AF_INET;
    //   hints.ai_socktype = SOCK_STREAM;
    //   hints.ai_protocol = IPPROTO_TCP;
    //
    //   int getaddrResult = getaddrinfo( url.Host,
    //         *url.Port ? url.Port : "443", &hints, &address );
    //
    //   //TEST stuff
    //   //PrintAddress( ( sockaddr_in * )address->ai_addr,
    //        //sizeof( struct sockaddr ) );
    //
    //   // Create a TCP/IP socket.
    //   int s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    //   assert( s != -1 );
    //
    //   // Connect the socket to the host address.
    //   int connectResult = connect( s, address->ai_addr,
    //         sizeof( struct sockaddr ) );
    //   assert( connectResult == 0 );
    //
    //   // Build an SSL layer and set it to read/write
    //   // to the socket we've connected.
    //   SSL_library_init( );
    //
    //   SSL_CTX *ctx = SSL_CTX_new( SSLv23_method( ) );
    //   assert( ctx );
    //   SSL *ssl = SSL_new( ctx );
    //   assert( ssl );
    //
    //   SSL_set_fd( ssl, s );
    //
    //   int sslConnectResult = SSL_connect( ssl );
    //   assert( sslConnectResult == 1 );
    //
    //   // Send a GET message.
    //   std::string getMessage;
    //   getMessage += "GET /";
    //   getMessage += url.Path;
    //   getMessage += " HTTP/1.1\r\n";
    //   getMessage += "Host: ";
    //   getMessage += url.Host;
    //   getMessage += "\r\n";
    //   getMessage += "User-Agent: LinuxGetUrl/2.0 meyersch@umich.edu (Linux)\r\n";
    //   getMessage += "Accept: */*\r\n";
    //   getMessage += "Accept-Encoding: identity\r\n";
    //   getMessage += "Connection: close\r\n\r\n";
    //
    //   SSL_write( ssl , getMessage.c_str( ), getMessage.length( ) );
    //
    //   // Read from the socket until there's no more data, copying it to
    //   // stdout.
    //   char buffer[ 10241 ];
    //   int bytes;
    //   int first_time = true;
    //
    //   std::string downloaded_html;
    //
    //   while ( /*cout << endl << endl << "BEGIN LOOP!" << endl << endl,*/ ( bytes = SSL_read( ssl, buffer, sizeof( buffer ) - 1 ) ) > 0 ){
    //      buffer[ 10241 ] = '\0';
    //      std::string s_buffer(buffer);
    //      int rnrn = s_buffer.find("\r\n\r\n");
    //      if (rnrn >= 0 && first_time) {
    //         first_time = false;
    //         downloaded_html += s_buffer.substr(rnrn + 4);
    //      }
    //      //else write( 1, buffer, bytes );
    //      else downloaded_html += std::string(buffer, bytes);
    //   }
    //
    //   //cout << buffer << endl;
    //
    //   // Close the socket and free the address info structure.
    //   SSL_shutdown( ssl );
    //   SSL_free( ssl );
    //   SSL_CTX_free( ctx );
    //
    //   close( s );
    //   freeaddrinfo( address );
    //
    //   return downloaded_html;
    //
    //   }
    //
    //}; //fb
}
