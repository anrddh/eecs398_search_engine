#include "bolt/http/http_connection_handler_factory.hpp"

fb::UniquePtr<HttpConnectionHandler>
HttpConnectionHandlerFactory::getHttpConnectionHandler() {
  return fb::makeUnique<SocketHttpConnectionHandler>();
}
