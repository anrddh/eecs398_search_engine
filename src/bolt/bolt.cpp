#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "fb/thread.hpp"

#include "bolt/bolt.hpp"
#include "bolt/bolt_impl.hpp"
#include "bolt/buffered_reader.hpp"
#include "bolt/constants.hpp"
#include "bolt/http/http_connection_handler.hpp"
#include "bolt/http/http_connection_handler_factory.hpp"
#include "bolt/http/http_request.hpp"

struct ThreadRequestInfo {
   fb::UniquePtr<HttpConnection> connection;
   fb::UniquePtr<HttpConnectionHandler> &connHandler;
   BoltImpl &boltImpl;
};

Bolt::Bolt()
    : pImpl(fb::makeUnique<BoltImpl>(
          HttpConnectionHandlerFactory::getHttpConnectionHandler())) {}

Bolt::Bolt(Bolt &&other) : pImpl(std::move(other.pImpl)) {}

Bolt Bolt::operator=(Bolt &&other) 
   {
   if (this != &other) 
      {
      pImpl = std::move(other.pImpl);
      }

   return std::move(*this);
   }

Bolt::~Bolt() = default;

void Bolt::run() { pImpl->run(); }

void Bolt::registerHandler( fb::String path, HtmlPage ( *page ) ( fb::UnorderedMap<fb::String, fb::String> ) ) 
   {
   return pImpl->registerHandler( path, page );
   }

void Bolt::setDefaultPath( HtmlPage ( *page ) ( ) ) 
   {
   return pImpl->setDefaultPath( page );
   }

namespace {
const fb::String createResponse(HtmlPage &page) 
   {
   fb::String html = page.getPageHtml();
   return fb::String(HTTP_VERSION) + " " + fb::toString(static_cast<int>(page.header.status)) + "\n"
      + "Content-Type: " + page.header.ctype + "\n"
      + "Content-Length: " + fb::toString(html.size()) + "\n"
      + "\n"
      + html;
   }
} // namespace

void * BoltImpl::respondToRequest(void * arg)
   {
   ThreadRequestInfo &info = *(ThreadRequestInfo *) arg;

   HttpRequest request(info.connection);

   HtmlPage page = info.boltImpl.getHtmlPage( request.getPath( ), request.getFormOptions( ) );

   fb::String response = createResponse(page);
   fb::UniquePtr<char[]> responseBuf =
         fb::makeUnique<char[]>(response.size());
   memcpy(responseBuf.get(), response.data(), response.size());
   info.connection->setRawResponse(std::move(responseBuf), response.size());
   info.connHandler->sendResponse(std::move(info.connection));

   delete ( ThreadRequestInfo * ) arg;
   return nullptr;
   }

HtmlPage BoltDefaultDefaultPage( ) 
   {
   HtmlPage page;
   page.loadFromString( "<h1>Page is not found!</h1>" );
   page.header.status = HttpStatus::NOT_FOUND;
   return page;
   }

BoltImpl::BoltImpl( fb::UniquePtr<HttpConnectionHandler> ch )
    : default_page( BoltDefaultDefaultPage ), connHandler( std::move( ch ) ) { }

void BoltImpl::run( ) 
   {
   connHandler->setup( );
   while ( true ) 
      {
      serveNextRequest( );
      }
   }

void BoltImpl::serveNextRequest( ) 
   {
   try 
      {
      fb::UniquePtr<HttpConnection> conn = getNextConnection();
      fb::Thread thread( &respondToRequest, new ThreadRequestInfo{ std::move( conn ), connHandler, *this } );
      thread.detach( );
      } 
   catch ( fb::Exception e) 
      {

      }
   }

fb::UniquePtr<HttpConnection> BoltImpl::getNextConnection() 
   {
   return connHandler->getRequest();
   }

void BoltImpl::registerHandler( fb::String path, HtmlPage ( func ) ( fb::UnorderedMap<fb::String, fb::String> ) ) 
{
   if (mappings.find(path) != mappings.end()) 
      {
      throw fb::Exception( "That handle was already registered!" );
      }
   else
      {
      mappings[path] = func;
      }
   }

void BoltImpl::setDefaultPath( HtmlPage( func )( ) ) { default_page = func; }

const HtmlPage BoltImpl::getHtmlPage( fb::String path, fb::UnorderedMap<fb::String, fb::String> formOptions ) {
   if ( mappings.find( path ) != mappings.end( ) ) 
      {
      return mappings[path](formOptions);
      } 
   else 
      {
      fb::String stripped_path(path.data( ) + 1);
      int f = open(stripped_path.data( ), O_RDWR);
      if(f > 0) 
         {
         close(f);
         HtmlPage page;
         page.loadRawFile(stripped_path);
         page.header.ctype = "image/png";
         return page;
         }  
      else
         {
         return default_page();
         }
   }
}
