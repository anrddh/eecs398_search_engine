#include <fstream>
#include <iostream>
#include <sstream>

#include "bolt/bolt.hpp"
#include "bolt/bolt_impl.hpp"
#include "bolt/buffered_reader.hpp"
#include "bolt/constants.hpp"
#include "bolt/http/http_connection_handler.hpp"
#include "bolt/http/http_connection_handler_factory.hpp"
#include "bolt/http/http_request.hpp"

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

void Bolt::registerHandler(fb::String path, HtmlPage (*page)()) 
   {
   return pImpl->registerHandler(path, page);
   }

void Bolt::setDefaultPath(HtmlPage (*page)()) 
   {
   return pImpl->setDefaultPath(page);
   }

namespace {
const fb::String createResponse(HtmlPage page) 
   {
   fb::String html = page.getPageHtml();
   return fb::String(HTTP_VERSION) + " " + fb::toString(static_cast<int>(page.header.status)) + "\n"
      + "Content-Type: " + page.header.ctype + "\n"
      + "Content-Length: " + fb::toString(html.size()) + "\n"
      + "\n"
      + html;
   }

}  // namespace

HtmlPage BoltDefaultDefaultPage() 
   {
   HtmlPage page;
   page.loadFromString("<h1>Page is not found!</h1>");
   page.header.status = HttpStatus::NOT_FOUND;
   return page;
   }

BoltImpl::BoltImpl(fb::UniquePtr<HttpConnectionHandler> ch)
    : default_page(BoltDefaultDefaultPage), connHandler(std::move(ch)) {}

void BoltImpl::run() 
   {
   connHandler->setup();
   while (true) 
      {
      serveNextRequest();
      }
   }

void BoltImpl::serveNextRequest() 
   {
   try 
      {
      fb::UniquePtr<HttpConnection> conn = getNextConnection();

      HttpRequest request(conn);

      HtmlPage page = getHtmlPage(request.getPath());

      fb::String response = createResponse(page);
      fb::UniquePtr<char[]> responseBuf =
            fb::makeUnique<char[]>(response.size());
      memcpy(responseBuf.get(), response.data(), response.size());
      conn->setRawResponse(std::move(responseBuf), response.size());
      connHandler->sendResponse(std::move(conn));
      }
   catch(fb::Exception e) 
      {
      std::cout << "Error: " << e.what() << std::endl;
      }
   }

fb::UniquePtr<HttpConnection> BoltImpl::getNextConnection() 
   {
   return connHandler->getRequest();
   }

void BoltImpl::registerHandler(fb::String path, HtmlPage(func)()) 
{
   if (mappings.find(path) != mappings.end()) 
      {
      throw fb::Exception("That handle was already registered!");
      }
   else
      {
      mappings[path] = func;
      }
   }

void BoltImpl::setDefaultPath(HtmlPage(func)()) { default_page = func; }

const HtmlPage BoltImpl::getHtmlPage(fb::String path) {
  if (mappings.find(path) != mappings.end()) {
    return mappings[path]();
  } else {
    return default_page();
  }
}
