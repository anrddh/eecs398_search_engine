#pragma once

#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <set>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// URL wrapper class 
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

      // print function for debugging
      void print( )
         {
         std::cout << "Complete Url = " << CompleteUrl << std::endl;
         std::cout << "Service = " << Service
               << ", Host = " << Host << ", Port = " << Port 
               << ", Path = " << Path << std::endl;
         }
   };

// Default port for https
const std::string ParsedUrl::defaultPort = "443";

// Wrapper class to handle writing
// Handles chunked encoding
class BufferWriter
{
   public:
      bool chunked;
      size_t chunkSize;
      std::string chunkSizeString;
      int fd;

      BufferWriter( bool chunkedIn, const std::string &filename )
      : chunked( chunkedIn ), chunkSize( 0 ), chunkSizeString( "" ) 
         {
         fd = creat( filename.c_str( ), 0666 );
         }

      ~BufferWriter( )
         {
         close( fd );
         }

      // get rid of \r\n in front
      void truncateFront( )
         {
         if ( chunkSizeString.length() >= 2 )
            {
            if ( chunkSizeString[ 0 ] == '\r' && chunkSizeString[ 1 ] == '\n' )
               {
               chunkSizeString = chunkSizeString.substr( 2 );
               }
            }
         }

      // get rid of \r\n at the back of the string
      // and get the string that represents size of the chunk
      bool truncateBack( )
         {
         if ( chunkSizeString.length() >= 2 )
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

      // main print function
      void print( char buffer[ ], int bytes )
      {
         if ( chunked )
            {
            for ( int i = 0;  i < bytes;  ++i )
               {
               // if chunk exhausted
               if ( chunkSize == 0 )
                  {
                  // find the next chunk size
                  chunkSizeString.push_back( buffer[ i ] );
                  truncateFront( );
                  if ( truncateBack( ) )
                     {
                     chunkSize = std::stoi( chunkSizeString, nullptr, 16 );
                     chunkSizeString = "";
                     }
                  }
               // write until chunk exhausted
               else
                  {
                  write( fd, buffer + i, 1 );
                  --chunkSize;
                  }
               }
            }
         else
            write( fd, buffer, bytes );
      }
   };

// Wrapper class to handle http
class ConnectionWrapper
   {
   public:
      ParsedUrl &url;
      int socketFD;

      // http connection
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
         if ( socketFD == -1 )
            recordFailedLink( );

         // Connect the socket to the host address
         int connectResult = connect( socketFD,
               address->ai_addr, address->ai_addrlen );
         if ( connectResult != 0 )
            recordFailedLink( );

         freeaddrinfo( address );
         }

      void recordFailedLink( )
         {
         int fd = open( "failed_links.txt", O_WRONLY | O_APPEND | O_CREAT, 0666 );
         ::write( fd, ( url.CompleteUrl + "\n" ).c_str( ),
               url.CompleteUrl.length( ) + 1 );
         close( fd );
         std::cerr << "Failed connecting to link: " << url.CompleteUrl << std::endl;
         exit( 1 );
         }

      virtual ~ConnectionWrapper( )
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

// Wrapper class to handle https
class SSLWrapper : public ConnectionWrapper
   {
   public:
      SSLWrapper( ParsedUrl &url_in )
      : ConnectionWrapper( url_in ) 
         {
         // Add SSL layer around http
         SSL_library_init( );
         OpenSSL_add_all_algorithms( );

         ctx = SSL_CTX_new( SSLv23_method( ) );
         if ( !ctx )
            recordFailedLink( );

         ssl = SSL_new( ctx );
         if ( !ssl )
            recordFailedLink( );

         SSL_set_fd( ssl, socketFD );

         // Needed for SNI websites
         int r = SSL_set_tlsext_host_name( ssl, url_in.Host.c_str( ) );
         if ( r != 1 )
            recordFailedLink( );

         r = SSL_connect( ssl );
         if ( r != 1 )
            recordFailedLink( );
         }

      virtual int read( char *buffer )
         {
         return SSL_read( ssl, buffer, sizeof ( buffer ) );
         }

      virtual int write( const std::string &message )
      {
         return SSL_write( ssl, message.c_str( ), message.length( ) );
      }

      virtual ~SSLWrapper( )
         {     
         SSL_shutdown( ssl );
         SSL_free( ssl );
         SSL_CTX_free( ctx );
         }

   private:
      SSL_CTX *ctx;
      SSL *ssl;
   };

// GetMessage
const std::string GetGetMessage( const ParsedUrl &url )
   {
   std::string getMessage = 
         "GET /" + url.Path + " HTTP/1.1\r\nHost: " + url.Host + "\r\n"
         "User-Agent: LinuxGetSsl/2.0 jinihm@umich.edu (Linux)\r\n"
         "Accept: */*\r\n"
         "Accept-Encoding: identity\r\n"
         "Connection: close\r\n\r\n";

   return getMessage;
   }

const std::string linkNotHTML = "LINK IS NOT HTML";

// Get relevant information from the header
// print received message after the header if no redirect 
// return the URL to redirect to if necessary
std::string parseHeader( ConnectionWrapper *connector, BufferWriter &writer )
   {
   char buffer [ 10240 ];
   int bytes;
   const std::string endHeader = "\r\n\r\n";
   std::string header = "    ";
   bool pastHeader = false;

   const std::string redirectIndicator = "Location: ";
   const std::string chunkedIndicator = "chunked";
   const std::string htmlIndicator = "Content-Type: text/html";
   std::string redirectUrl = "";

   while ( ( bytes =  connector->read(buffer)) > 0 )
   {
      // build up header
      for ( int i = 0;  i < bytes;  ++i )
      {
      header.push_back( buffer[ i ] );

      // if end of header reached
      if ( std::string( header.end( ) - 4, header.end( ) ) == endHeader )
         {
         // check for chunked
         if ( header.find( chunkedIndicator ) != std::string::npos )
            writer.chunked = true;

         // check if html
         if ( header.find( htmlIndicator ) == std::string::npos )
            return linkNotHTML;

         // check for redirect
         size_t startRedirectUrl = header.find( redirectIndicator );
         if ( startRedirectUrl != std::string::npos )
            {
            size_t endRedirectUrl = header.find( "\r\n", startRedirectUrl );
            redirectUrl = header.substr(
                  startRedirectUrl + redirectIndicator.length( ),
                  endRedirectUrl - startRedirectUrl - redirectIndicator.length( ) );
            }
         else
         {
            std::cout << header << std::endl;
            // print the remaining message if no need to redirect
            writer.print( buffer + i + 1, bytes - i - 1 );
         }
         
         pastHeader = true;
         break;
         }
      }
      if ( pastHeader )
         break;
   }
   return redirectUrl;
   }

// Helper function to get the correct ConnectionWrapper
ConnectionWrapper * ConnectionWrapperFactory( ParsedUrl &url )
   {
   if ( url.Service == "http" )
      return new ConnectionWrapper( url );
   else
      return new SSLWrapper( url );
   }

// Esatblish connection with the url
// If redirect, return the redirect url
// Else, write the recieved content to a file
std::string PrintHtmlGetRedirect( const std::string &url_in, const std::string &filename )
   {
      // Parse the URL
   ParsedUrl url( url_in );

   ConnectionWrapper *connector = ConnectionWrapperFactory( url );

   // Send a GET message
   std::string getMessage = GetGetMessage( url );

   int sendResult = connector->write( getMessage );
   assert( sendResult != - 1 );

   // Read from the socket
   char buffer [ 10240 ];
   int bytes;

   // temporary filename
   BufferWriter writer( false , filename );

   // Check for redirect and other relevant header info
   std::string redirectUrl = parseHeader( connector, writer );
   
   if ( redirectUrl != linkNotHTML )
      {
      // If no redirect, write the content
      if ( redirectUrl == "" )
         {
         while ( ( bytes =  connector->read(buffer)) > 0 )
            {
            writer.print( buffer, bytes );
            }
         }
      }

   // clean up
   delete connector;

   return redirectUrl;
   }

// Write the url information to a file
// handling redirect appropriately
// If redirect creates a loop, do nothing
void PrintHtml( const std::string &url_in, const std::string &filename )
   {
   std::set<std::string> visitedURLs;

   visitedURLs.insert( url_in );

   std::string url = url_in;

   while ( url.length( ) != 0 )
      {
      url = PrintHtmlGetRedirect( url, filename );

      // If there is a loop, probably a bad website.
      if( visitedURLs.find( url ) != visitedURLs.end( ) )
         break;

      visitedURLs.insert( url );
      }
   }
