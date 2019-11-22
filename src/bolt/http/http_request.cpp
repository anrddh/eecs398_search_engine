#include "bolt/http/http_request.hpp"

namespace {
HttpType getHttpType(fb::String type) {
  if (type == "GET") {
    return HttpType::GET;
  } else if (type == "POST") {
    return HttpType::POST;
  } else {
    return HttpType::OTHER;
  }
}
}  // namespace

HttpRequest::HttpRequest(std::unique_ptr<HttpConnection>& conn) {
  const std::unique_ptr<char[]>& rawReq = conn->getRawRequest();
  int length = conn->getRawRequestLength();
  BufferedReader bf(rawReq, length);
  parseType(bf);
  parsePath(bf);
  parseHeaders(bf);
}

HttpType HttpRequest::getType() { return httpType; }

fb::String HttpRequest::getPath() { return path; }

fb::String HttpRequest::getHeader(fb::String headerKey) {
  if (headers.find(headerKey) == headers.end()) {
    throw std::runtime_error("header not in specified in request");
  } else {
    return headers[headerKey];
  }
}

void HttpRequest::parseType(BufferedReader& bf) {
  fb::String typeStr = bf.nextWord();
  httpType = getHttpType(typeStr);
}

void HttpRequest::parsePath(BufferedReader& bf) { path = bf.nextWord(); }

void HttpRequest::parseHeaders(BufferedReader& bf) {
  while (!bf.pastEnd()) {
    fb::String headerKey = bf.nextWord();
    fb::String headerValue = bf.nextWord();

    // strip trailing colon on key and  append to map
    headers[headerKey.substr(0, headerKey.size() - 1)] = headerValue;
  }
}