#pragma once

#include "fb/memory.hpp"

#include "bolt/http/socket_http_connection_handler.hpp"

class HttpConnectionHandlerFactory 
   {
public:
   static fb::UniquePtr<HttpConnectionHandler> getHttpConnectionHandler();
   };
