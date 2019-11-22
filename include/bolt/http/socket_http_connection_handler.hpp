#pragma once

#include <memory>

#include "bolt/http/http_connection_handler.hpp"

class SocketHttpConnection : public HttpConnection {
 public:
  SocketHttpConnection(std::unique_ptr<char[]> rawRequest, int len, int sock);

  std::unique_ptr<char[]>& getRawRequest();
  int getRawRequestLength();

  void setRawResponse(std::unique_ptr<char[]> response, int len);

  std::unique_ptr<char[]> getRawResponse();
  int getRawResponseLength();

  int getClientHandle();

 private:
  std::unique_ptr<char[]> responseBuf;
  std::unique_ptr<char[]> requestBuf;
  int length;
  int clientHandle;
};

class SocketHttpConnectionHandler : public HttpConnectionHandler {
 public:
  void setup();
  std::unique_ptr<HttpConnection> getRequest();
  void sendResponse(std::unique_ptr<HttpConnection> conn);

 private:
  int serverHandle;
};
