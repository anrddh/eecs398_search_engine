#pragma once

#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../lib/stddef.hpp"
#include "../../lib/unordered_set.hpp"
#include "../../lib/string.hpp"
#include "../../lib/utility.hpp"

// URL wrapper class 
class ParsedUrl
   {
   public:
      static const fb::String defaultPort; 
      const fb::String CompleteUrl;
      fb::String Service, Host, Port, Path;

      ParsedUrl( const fb::String &url )
        : CompleteUrl( url )
         {
         fb::SizeT start = 0;
         fb::StringView CompleteUrlView( CompleteUrl );
         fb::SizeT end = CompleteUrlView.find( "://", 0, 3 );
         if ( end != fb::String::npos )
            {
            Service = CompleteUrl.substr( start, end - start );
            start = end + 3;
            }

         end = CompleteUrlView.find( "/", start, 1 );
         fb::SizeT HostEnd = CompleteUrlView.find( ":", start, 1 );
         if ( HostEnd < end )
            {
            Host = CompleteUrl.substr( start, HostEnd - start );
            Port = CompleteUrl.substr( HostEnd + 1, end - HostEnd - 1 );
            }
         else
            {
            if ( end == fb::String::npos )
               Host = CompleteUrl.substr( start );
            else
               Host = CompleteUrl.substr( start, end - start );

            if ( Service == "http" )
               Port = "80";
            else if ( Service == "https" )
               Port = "443";
            else
               Port = defaultPort;
            }

         if ( end != fb::String::npos )
            Path = CompleteUrl.substr( end + 1 );
         else
            Path = "";
         }

      ~ParsedUrl( ) 
         {
         };

      // print function for debugging
      void print( ) const
         {
         std::cout << "Complete Url = " << CompleteUrl << std::endl;
         std::cout << "Service = " << Service
               << ", Host = " << Host << ", Port = " << Port 
               << ", Path = " << Path << std::endl;
         }
   };

// Default port for https
const fb::String ParsedUrl::defaultPort = "443";

// Wrapper class to handle writing
// Handles chunked encoding
class BufferWriter
   {
   public:
      fb::String downloadedContent;
      bool chunked;

      BufferWriter( bool chunkedIn )
      : chunked( chunkedIn ), chunkSize( 0 ), chunkSizeString( "" )
         {
         }

      ~BufferWriter( )
         {
         }

      void addContent( const char buffer[ ], int bytes )
         {
         downloadedContent.append( { buffer, static_cast<fb::SizeT>(bytes) } );
         }

      bool contentTooBig( )
         {
         return downloadedContent.size( ) > 1e+7;
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
                  chunkSizeString += buffer[ i ];
                  truncateFront( );
                  if ( truncateBack( ) )
                     {
                     chunkSize = fb::stoi( chunkSizeString, nullptr, 16 );
                     chunkSizeString = "";
                     }
                  }
               // write until chunk exhausted
               else
                  {
                  addContent( buffer + i, 1 );
                  --chunkSize;
                  }
               }
            }
         else
         {
            addContent( buffer, bytes );
         }
      }

   private:
      fb::SizeT chunkSize;
      fb::String chunkSizeString;


      // get rid of \r\n in front
      void truncateFront( )
         {
         if ( chunkSizeString.size( ) >= 2 )
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
         if ( chunkSizeString.size() >= 2 )
            {
            if ( chunkSizeString[chunkSizeString.size( ) - 2 ] == '\r' 
                  && chunkSizeString[chunkSizeString.size( ) - 1 ] == '\n' )
               {
               chunkSizeString = chunkSizeString.substr(
                     0, chunkSizeString.size( ) - 2 );
               return true;
               }
            }
         return false;
         }
   };

struct ConnectionAcception 
   {
   ConnectionAcception(const fb::String msg_) : msg(msg_)
      {
      }

   const fb::String msg;
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
         int getaddrResult = getaddrinfo( url.Host.data( ),
               url.Port.data( ), &hints, &address );

         if ( getaddrResult != 0 )
            recordFailedLink( "getaddrResult" );

         // Create a TCP/IP socket
         socketFD = socket( address->ai_family,
               address->ai_socktype, address->ai_protocol );
         if ( socketFD == -1 )
            recordFailedLink( "socket" );

         // Connect the socket to the host address
         int connectResult = connect( socketFD,
               address->ai_addr, address->ai_addrlen );
         if ( connectResult != 0 )
            recordFailedLink( "connectResult" );

         freeaddrinfo( address );
         }

      void recordFailedLink( fb::String msg )
         {
         int fd = open( "failed_links.txt", O_WRONLY | O_APPEND | O_CREAT, 0666 );
         ::write( fd, ( url.CompleteUrl + "\n" ).data( ),
               url.CompleteUrl.size( ) + 1 );
         close( fd );
         std::cerr << "Failed connecting to link: " << url.CompleteUrl << std::endl;
         std::cerr << "Failed at " << msg << std::endl;
         throw ConnectionAcception( msg );
         }

      virtual ~ConnectionWrapper( )
         {
         close( socketFD );
         }

      virtual int read( char *buffer ) 
         {
         return recv( socketFD, buffer, sizeof ( buffer ), 0 );
         }

      virtual int write( const fb::String &message )
         {
         return send( socketFD, message.data( ), message.size( ), 0 );
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
            recordFailedLink( "ssl ctx" );

         ssl = SSL_new( ctx );
         if ( !ssl )
            recordFailedLink( "ssl" );

         SSL_set_fd( ssl, socketFD );

         // Needed for SNI websites
         int r = SSL_set_tlsext_host_name( ssl, url_in.Host.data( ) );
         if ( r != 1 )
            recordFailedLink( "ssl set host name" );

         r = SSL_connect( ssl );
         if ( r != 1 )
            recordFailedLink( "ssl connect" );
         }

      virtual int read( char *buffer )
         {
         return SSL_read( ssl, buffer, sizeof ( buffer ) );
         }

      virtual int write( const fb::String &message )
      {
         return SSL_write( ssl, message.data( ), message.size( ) );
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

class HTTPDownloader
{
public:

HTTPDownloader( )
   : header( "    " ) 
   {
   }

fb::String redirectUrl;
fb::String header;

// GetMessage
const fb::String GetGetMessage( const ParsedUrl &url )
   {
   fb::String getMessage = 
         "GET /" + url.Path + " HTTP/1.1\r\nHost: " + url.Host + "\r\n" +
         "User-Agent: LinuxGetSsl/2.0 (Linux)\r\n" +
         "Accept: */*\r\n" +
         "Accept-Encoding: identity\r\n" +
         "Connection: close\r\n\r\n"; 
   return getMessage;
   }

bool headerEnd( )
   {
   const fb::String endHeader = "\r\n\r\n";
   fb::SizeT offset = header.size( ) - 4;
   for ( int i = 0;  i < 4;  ++i )
      if ( header[ offset + i ] != endHeader[ i ] )
         return false;
   return true;
   }

bool checkFileType( const fb::StringView &headerView, const fb::Vector<fb::String> &acceptableTypes )
   {
   for ( const auto i : acceptableTypes )
      if ( headerView.find( i ) != fb::String::npos )
         return true;

   return false;
   }

struct HeaderResult
   {
   fb::String response;
   bool fileTypeGood;
   };

// Get header and parse relevant information
// return apporpriate pair of DownloadStatus and redirectUrl
HeaderResult parseHeader( ConnectionWrapper *connector, BufferWriter &writer, 
   const fb::Vector<fb::String> &acceptableTypes )
   {
   char buffer [ 10240 ];
   int bytes;
   bool pastHeader = false;

   // some websites do not use capital case.
   const fb::String redirectIndicator = "ocation: ";
   const fb::String chunkedIndicator = "chunked";
   const fb::String endLineIndicator = "\r\n";

   HeaderResult headerResult;
   headerResult.fileTypeGood = true;

   header = "    ";
   redirectUrl.clear();

   while ( ( bytes =  connector->read( buffer ) ) > 0 )
      {
      // build up header
      for ( int i = 0;  i < bytes;  ++i )
         {
         header += buffer[ i ];
         // if end of header reached
         if ( headerEnd( ) )
            {
            fb::SizeT firstSpace = header.find( ' ', 4 );
            fb::SizeT secondSpace = header.find( ' ', firstSpace );
            headerResult.response = header.substr(firstSpace + 1, secondSpace - firstSpace - 1);

            // check for chunked
            const fb::StringView headerView( header );
            if ( headerView.find( chunkedIndicator ) != fb::String::npos )
               writer.chunked = true;

            // check for redirect
            fb::SizeT startRedirectUrl = headerView.find( redirectIndicator );
            if ( startRedirectUrl != fb::String::npos )
               {
               fb::SizeT endRedirectUrl = headerView.find( endLineIndicator, startRedirectUrl );
               redirectUrl = header.substr(
                     startRedirectUrl + redirectIndicator.size( ),
                     endRedirectUrl - startRedirectUrl - redirectIndicator.size( ) );
               }
            else if ( !checkFileType( headerView, acceptableTypes ) )
               headerResult.fileTypeGood = false;
            else
               {
               // print the remaining message if no need to redirect
               fb::String url_comment = "<!-- " + connector->url.CompleteUrl + " -->\n";
               writer.addContent( url_comment.data( ), url_comment.size( ) );
               writer.print( buffer + i + 1, bytes - i - 1 );
               }
            
            pastHeader = true;
            break;
            }
         }
      if ( pastHeader )
         break;
      }
   return headerResult;
   }

// Helper function to get the correct ConnectionWrapper
ConnectionWrapper * ConnectionWrapperFactory( ParsedUrl &url )
   {
   if ( url.Service == "http" )
      return new ConnectionWrapper( url );
   else
      return new SSLWrapper( url );
   }

struct DownloadResult 
{
   fb::String response;
   fb::String downloadedContent;
   bool fileTypeGood;
   bool fileSmallEnough;
};

// Esatblish connection with the url and try to write to downloadedContent
// Only download if type of the content matches and not too big
// return apporpriate pair of DownloadStatus and redirectUrl
DownloadResult PrintGetRedirect( const fb::String &url,
   const fb::Vector<fb::String> &acceptableTypes )
   {
      // Parse the URL
   ParsedUrl parsedUrl( url );

   ConnectionWrapper *connector = ConnectionWrapperFactory( parsedUrl );

   // Send a GET message
   fb::String getMessage = GetGetMessage( parsedUrl );

   int sendResult = connector->write( getMessage );
   if ( sendResult == -1 )
      throw ConnectionAcception( "Cannot write" );

   // Read from the socket
   char buffer [ 10240 ];
   int bytes;

   // temporary filename
   BufferWriter writer( false );

   // Check for redirect and other relevant header info
   HeaderResult headerResult = parseHeader( connector, writer, acceptableTypes );

   DownloadResult dlResult;
   dlResult.response = headerResult.response;
   dlResult.fileTypeGood = headerResult.fileTypeGood;
   dlResult.fileSmallEnough = true;

   // std::cout << "Sadas" << std::endl;
   // std::cout << headerResult.response << "ss " << std::endl;
   // std::cout << redirectUrl << std::endl;

   if ( headerResult.response == "301"
         && ( !redirectUrl.empty() && redirectUrl[ 0 ] == '/' ) )
      redirectUrl = parsedUrl.Service + "://" + parsedUrl.Host + redirectUrl;
   else if ( headerResult.response != "301" )
      {
      // write the content if no redirect and link is html
      if ( headerResult.fileTypeGood )
         {
         while ( ( bytes =  connector->read( buffer ) ) > 0 
               && !writer.contentTooBig( ) )
         {
            // std::cout << "downloading" << std::endl;
            // std::cout << bytes << std::endl;
            writer.print( buffer, bytes );
         }

         }

      if ( !writer.contentTooBig() )
         {
         // std::cout << "not too big" << std::endl;
         dlResult.downloadedContent = writer.downloadedContent;
         dlResult.fileSmallEnough = true;
         }
      else
         dlResult.fileSmallEnough = false;
      }

   // clean up
   delete connector;

   return dlResult;
   }

// Write the content of the url except header to donloadedContent
// redirect is handled to the extent that there is no loop in redirect
// type of the content in url must match acceptableTypes
// return true if download succeeds
DownloadResult PrintFile( const fb::String &url,
      const fb::Vector<fb::String> &acceptableTypes )
   {
   fb::UnorderedSet<fb::String> visitedURLs;

   visitedURLs.insert( url );

   DownloadResult result;
   result.response = "301";
   redirectUrl = url;

   while ( result.response == "301" )
      {
         // std::cout << "before redirect " << redirectUrl  << std::endl;
      result = PrintGetRedirect( redirectUrl, acceptableTypes );
      // std::cout << "redirect " << redirectUrl  << std::endl;
      // std::cout << result.downloadedContent << std::endl;
      // If there is a loop, probably a bad website.
      if ( !result.fileSmallEnough || !result.fileTypeGood ||
         ( result.response == "301" && visitedURLs.find( redirectUrl ) != visitedURLs.end( ) ) )
         {
            // std::cout  << "in break" << std::endl;
            // std::cout << !result.fileTypeGood << std::endl;
         break;
         }

      visitedURLs.insert( redirectUrl );
      }

   return result;
   }

// Downlaod html file of given url handling redirect appropriately,
// and append the content to downloadedContent
// downloadedContent will be cleared at beginning and if download fails
// return true if donwload succeeds
// exception thrown if cannot connect to url
DownloadResult PrintHtml( const fb::String &url_in )
   {
   fb::Vector<fb::String> acceptableTypes = { "text/html" };
   return PrintFile( url_in, acceptableTypes );
   }

// Downlaod html file or txt file of given url handling redirect appropriately,
// and append the content to downloadedContent
// downloadedContent will be cleared at beginning and if download fails
// return true if donwload succeeds
// exception thrown if cannot connect to url
DownloadResult PrintPlainTxt( const fb::String &url_in )
   {
   fb::Vector<fb::String> acceptableTypes = { "text/html", "text/plain" };
   return PrintFile( url_in, acceptableTypes );
   }

};

