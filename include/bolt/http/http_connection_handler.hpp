#pragma once

#include "fb/memory.hpp"

class HttpConnection {
 public:
  virtual const fb::UniquePtr<char[]>& getRawRequest() = 0;
  virtual int getRawRequestLength() = 0;
  virtual void setRawResponse(fb::UniquePtr<char[]> response, int length) = 0;
  virtual ~HttpConnection() {}
};

class HttpConnectionHandler {
 public:
  virtual void setup() = 0;
  virtual fb::UniquePtr<HttpConnection> getRequest() = 0;
  virtual void sendResponse(fb::UniquePtr<HttpConnection> conn) = 0;
  virtual ~HttpConnectionHandler() {}
};
