#pragma once

#include "fb/memory.hpp"

#include "bolt/http/http_connection_handler.hpp"

class SocketHttpConnection : public HttpConnection {
 public:
  SocketHttpConnection(fb::UniquePtr<char[]> rawRequest, int len, int sock);

  fb::UniquePtr<char[]>& getRawRequest();
  int getRawRequestLength();

  void setRawResponse(fb::UniquePtr<char[]> response, int len);

  fb::UniquePtr<char[]> getRawResponse();
  int getRawResponseLength();

  int getClientHandle();

 private:
  fb::UniquePtr<char[]> responseBuf;
  fb::UniquePtr<char[]> requestBuf;
  int length;
  int clientHandle;
};

class SocketHttpConnectionHandler : public HttpConnectionHandler {
 public:
  void setup();
  fb::UniquePtr<HttpConnection> getRequest();
  void sendResponse(fb::UniquePtr<HttpConnection> conn);

 private:
  int serverHandle;
};
