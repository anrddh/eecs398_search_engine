#pragma once

#include <memory>

#include "bolt/http/socket_http_connection_handler.hpp"

class HttpConnectionHandlerFactory 
   {
public:
   static std::unique_ptr<HttpConnectionHandler> getHttpConnectionHandler();
   };
