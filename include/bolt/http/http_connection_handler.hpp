#pragma once

#include <memory>

class HttpConnection {
 public:
  virtual const std::unique_ptr<char[]>& getRawRequest() = 0;
  virtual int getRawRequestLength() = 0;
  virtual void setRawResponse(std::unique_ptr<char[]> response, int length) = 0;
  virtual ~HttpConnection() {}
};

class HttpConnectionHandler {
 public:
  virtual void setup() = 0;
  virtual std::unique_ptr<HttpConnection> getRequest() = 0;
  virtual void sendResponse(std::unique_ptr<HttpConnection> conn) = 0;
  virtual ~HttpConnectionHandler() {}
};
