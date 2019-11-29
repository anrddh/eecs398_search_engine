#pragma once

#include "fb/memory.hpp"
#include "fb/string.hpp"
#include "fb/unordered_map.hpp"

#include "bolt/buffered_reader.hpp"
#include "bolt/http/http_connection_handler.hpp"

enum HttpType { GET, POST, OTHER };

class HttpRequest {
 public:
  HttpRequest(fb::UniquePtr<HttpConnection> &conn);
  HttpType getType();
  fb::String getPath();
  fb::String getHeader(fb::String headerKey);

 private:
  HttpType httpType;
  fb::String path;
  fb::UnorderedMap<fb::String, fb::String> headers;

  void parseType(BufferedReader &bf);
  void parsePath(BufferedReader &bf);
  void parseHeaders(BufferedReader &bf);
};
