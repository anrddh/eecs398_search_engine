#include "bolt/http/http_connection_handler_factory.hpp"

std::unique_ptr<HttpConnectionHandler>
HttpConnectionHandlerFactory::getHttpConnectionHandler() {
  return std::make_unique<SocketHttpConnectionHandler>();
}