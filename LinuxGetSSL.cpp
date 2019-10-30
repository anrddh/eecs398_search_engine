#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <set>

class ParsedUrl
   {
   public:
      static const std::string defaultPort; 
      const std::string CompleteUrl;
      std::string Service, Host, Port, Path;

      ParsedUrl( const std::string &url )
        : CompleteUrl( url )
         {
         size_t start = 0;
         size_t end = CompleteUrl.find( "://" );
         if ( end != std::string::npos )
            {
            Service = CompleteUrl.substr( start, end - start );
            start = end + 3;
            }

         end = CompleteUrl.find( "/", start );
         size_t HostEnd = CompleteUrl.find( ":", start );
         if ( HostEnd < end )
            {
            Host = CompleteUrl.substr( start, HostEnd - start );
            Port = CompleteUrl.substr( HostEnd + 1, end - HostEnd - 1 );
            }
         else
            {
            Host = CompleteUrl.substr( start, end - start );
            if ( Service == "http" )
               Port = "80";
            else if ( Service == "https" )
               Port = "443";
            else
               Port = defaultPort;
            }

         if ( end != std::string::npos )
            Path = CompleteUrl.substr( end + 1 );
         }

      ~ParsedUrl( ) 
         {
         };
   };

const std::string ParsedUrl::defaultPort = "443";

class BufferPrinter
{
   public:
      bool chunked;
      size_t chunkSize;
      std::string chunkSizeString;

      BufferPrinter( bool chunkedIn )
      : chunked( chunkedIn ), chunkSize( 0 ), chunkSizeString( "" ) 
         {
         }

      void truncateFront()
         {
         if ( chunkSizeString.length() >= 2)
            {
            if ( chunkSizeString[ 0 ] == '\r' && chunkSizeString[ 1 ] == '\n' )
               {
               chunkSizeString = chunkSizeString.substr( 2 );
               }
            }
         }

      bool truncateBack()
         {
         if ( chunkSizeString.length() >= 2)
            {
            if ( chunkSizeString[chunkSizeString.length( ) - 2 ] == '\r' 
                  && chunkSizeString[chunkSizeString.length( ) - 1 ] == '\n' )
               {
               chunkSizeString = chunkSizeString.substr(
                     0, chunkSizeString.length( ) - 2 );
               return true;
               }
            }
         return false;
         }

      void print( char buffer[ ], int bytes )
      {
         if ( chunked )
            {
            for ( int i = 0;  i < bytes;  ++i )
               {
               if ( chunkSize == 0 )
                  {
                  chunkSizeString.push_back( buffer[ i ] );
                  truncateFront( );
                  if ( truncateBack( ) )
                     {
                     chunkSize = std::stoi( chunkSizeString, nullptr, 16 );
                     chunkSizeString = "";
                     }
                  }
               else
                  {
                  std::cout << buffer[ i ];
                  --chunkSize;
                  }
               }
            }
         else
            write( 1, buffer, bytes );
      }
   };

class ConnectionWrapper
   {
   public:
      ParsedUrl &url;
      int socketFD;

      ConnectionWrapper( ParsedUrl &url_in )
      : url(url_in) 
         {
         // Get the host address
         struct addrinfo *address, hints;
         memset( &hints, 0, sizeof( hints ) );
         hints.ai_family = AF_INET;
         hints.ai_socktype = SOCK_STREAM;
         hints.ai_protocol = IPPROTO_TCP;
         int getaddrResult = getaddrinfo( url.Host.c_str( ),
               url.Port.c_str( ), &hints, &address );

         // Create a TCP/IP socket
         socketFD = socket( address->ai_family,
               address->ai_socktype, address->ai_protocol );
         assert( socketFD != -1 );

         // Connect the socket to the host address
         int connectResult = connect( socketFD,
               address->ai_addr, address->ai_addrlen );
         assert( connectResult == 0 );

         freeaddrinfo( address );
         }

      virtual ~ConnectionWrapper ()
         {
         close( socketFD );
         }

      virtual int read( char *buffer ) 
         {
         return recv( socketFD, buffer, sizeof ( buffer ), 0 );
         }

      virtual int write( const std::string &message )
         {
         return send( socketFD, message.c_str( ), message.length( ), 0 );
         }

   };

class SSLWrapper : public ConnectionWrapper
   {
   public:
      SSLWrapper( ParsedUrl &url_in )
      : ConnectionWrapper( url_in ) 
         {
         SSL_library_init( );

         ctx = SSL_CTX_new( SSLv23_method( ) );
         assert( ctx );
         ssl = SSL_new( ctx );
         assert( ssl );

         SSL_set_fd( ssl, socketFD );

         int sslConnectResult = SSL_connect( ssl );

         assert( sslConnectResult == 1 );
         }

      virtual int read( char *buffer )
         {
         return SSL_read( ssl, buffer, sizeof ( buffer ) );
         }

      virtual int write( const std::string &message )
      {
         return SSL_write( ssl, message.c_str( ), message.length( ) );
      }

      virtual ~SSLWrapper()
         {     
         SSL_shutdown( ssl );
         SSL_free( ssl );
         SSL_CTX_free( ctx );
         }

   private:
      SSL_CTX *ctx;
      SSL *ssl;
   };

void printParsedUrl( const ParsedUrl &url )
   {
   std::cout << "Complete Url = " << url.CompleteUrl << std::endl;
   std::cout << "Service = " << url.Service
         << ", Host = " << url.Host << ", Port = " << url.Port 
         << ", Path = " << url.Path << std::endl;
   }

std::string GetGetMessage( const ParsedUrl &url )
   {
   std::string getMessage = 
         "GET /" + url.Path + " HTTP/1.1\r\nHost: " + url.Host + "\r\n"
         "User-Agent: LinuxGetSsl/2.0 jinihm@umich.edu (Linux)\r\n"
         "Accept: */*\r\n"
         "Accept-Encoding: identity\r\n"
         "Connection: close\r\n\r\n";

   return getMessage;
   }

std::string PrintHtmlGetRedirect( const std::string &url_in )
{
      // Parse the URL
   ParsedUrl url( url_in );

   ConnectionWrapper *connector;

   if ( url.Service == "http" )
   {
      connector = new ConnectionWrapper( url );
   }
   else
   {
      connector = new SSLWrapper( url );
   }

   // Send a GET message
   std::string getMessage = GetGetMessage( url );

   int sendResult = connector->write( getMessage );
   assert( sendResult != - 1 );

   // Read from the socket
   char buffer [ 10240 ];
   int bytes;
   bool pastHeader = false;
   const std::string endHeader = "\r\n\r\n";
   std::string header = "    ";

   bool redirect = false;
   const std::string redirectIndicator = "Location: ";
   const std::string chunkedIndicator = "Transfer-Encoding: chunked";
   std::string redirectUrl = "";

   BufferPrinter printer(false);

   while ( ( bytes =  connector->read(buffer)) > 0 )
      {
      if ( !pastHeader )
         {
         for ( int i = 0;  i < bytes;  ++i )
            {
            header.push_back( buffer[ i ] );
            if ( std::string( header.end( ) - 4, header.end( ) ) == endHeader )
               {
               pastHeader = true;
               if ( header.find( chunkedIndicator ) != std::string::npos )
                  {
                  printer.chunked = true;
                  }

               size_t startRedirectUrl = header.find( redirectIndicator );
               if ( startRedirectUrl != std::string::npos )
                  {
                  redirect = true;
                  size_t endRedirectUrl = header.find( "\r\n", startRedirectUrl );
                  redirectUrl = header.substr(
                        startRedirectUrl + redirectIndicator.length( ),
                        endRedirectUrl - startRedirectUrl - redirectIndicator.length( ) );
                  break;
                  }

               printer.print( buffer + i + 1, bytes - i - 1 );
               break;
               }
            }
         }
      else
         {
         printer.print( buffer, bytes );
         }

      if ( redirect )
         break;
      }

   delete connector;

   return redirectUrl;
}

int main( int argc, char *argv[ ] ) {
   std::string url = argv[ 1 ];

   std::set<std::string> visitedURLs;

   visitedURLs.insert( url );

   while ( url.length( ) != 0 )
      {
      url = PrintHtmlGetRedirect( url );

      // If there is a loop, probably a bad website.
      if(visitedURLs.find( url ) != visitedURLs.end( ) )
        break;
      visitedURLs.insert( url );
      }
}

