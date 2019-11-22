#pragma once

#include <memory>
#include "fb/string.hpp"
#include "fb/unordered_map.hpp"

#include "bolt/html_page.hpp"
#include "bolt/http/http_connection_handler.hpp"
#include "bolt/http/http_connection_handler_factory.hpp"

class BoltImpl 
   {
public:
   BoltImpl(std::unique_ptr<HttpConnectionHandler> ch);

   void run();

   void registerHandler(fb::String, HtmlPage (*)());

   void setDefaultPath(HtmlPage (*)());

   void serveNextRequest();

private:
   fb::UnorderedMap<fb::String, HtmlPage (*)()> mappings;
   HtmlPage (*default_page)();

   std::unique_ptr<HttpConnectionHandler> connHandler;

   const HtmlPage getHtmlPage(fb::String);

   std::unique_ptr<HttpConnection> getNextConnection();
   };
