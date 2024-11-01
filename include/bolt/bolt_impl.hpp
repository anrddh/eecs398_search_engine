#pragma once

#include "fb/memory.hpp"
#include "fb/string.hpp"
#include "fb/unordered_map.hpp"

#include "bolt/html_page.hpp"
#include "bolt/http/http_connection_handler.hpp"
#include "bolt/http/http_connection_handler_factory.hpp"

class BoltImpl 
   {
public:
   BoltImpl( fb::UniquePtr<HttpConnectionHandler> ch );

   void run( );

   void registerHandler( fb::String, HtmlPage ( * )( fb::UnorderedMap<fb::String, fb::String> ) );

   void setDefaultPath( HtmlPage ( * ) ( ) );

   void serveNextRequest( );

private:
   fb::UnorderedMap<fb::String, HtmlPage ( * )( fb::UnorderedMap<fb::String, fb::String> )> mappings;
   HtmlPage ( *default_page ) ( );
   fb::UniquePtr<HttpConnectionHandler> connHandler;

   static void * respondToRequest( void * arg );

   const HtmlPage getHtmlPage( fb::String, fb::UnorderedMap<fb::String, fb::String> );

   fb::UniquePtr<HttpConnection> getNextConnection( );
   };
