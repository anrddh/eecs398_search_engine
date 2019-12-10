#include <bolt/http/http_request.hpp>

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

HttpRequest::HttpRequest(fb::UniquePtr<HttpConnection>& conn) {
  const fb::UniquePtr<char[]>& rawReq = conn->getRawRequest();
  int length = conn->getRawRequestLength();
  BufferedReader bf(rawReq, length);
  parseType(bf);
  parsePath(bf);
  parseHeaders(bf);
}

HttpType HttpRequest::getType() { return httpType; }

fb::String HttpRequest::getPath() { return path; }

fb::UnorderedMap<fb::String, fb::String> HttpRequest::getFormOptions( ) { return formOptions; }

fb::String HttpRequest::getHeader(fb::String headerKey) {
  if (headers.find(headerKey) == headers.end()) {
    throw std::runtime_error("header not in specified in request");
  } else {
    return headers[headerKey];
  }
}

void HttpRequest::parseType( BufferedReader& bf ) {
  fb::String typeStr = bf.nextWord( );
  httpType = getHttpType( typeStr );
}

void HttpRequest::parsePath( BufferedReader& bf )
  {
  fb::String fullPath = bf.nextWord( );
  char * start = fullPath.data( );
  auto iter = fullPath.begin( );
  for( ; iter != fullPath.end( ) && *iter != '?'; ++iter )
    ;
  path = fb::String( fullPath.begin( ), iter );
  if (*iter == '?')
      ++iter;
  fb::String options ( iter, fullPath.end( ) );
  if( !options.empty( ) )
    {
    auto iter = options.begin( );
    while( iter != options.end( ) )
      {
      auto keyStart = iter;
      for( ; iter != options.end( ) && *iter != '='; ++iter )
        ;

      fb::String key(keyStart, iter);

      if( iter != options.end( ) )
        {
        ++iter;
        }

      auto valueStart = iter;
      for( ; iter != options.end( ) && *iter != '&'; ++iter )
        ;
      fb::String value(valueStart, iter);
      if( iter != options.end( ) )
        {
        ++iter;
        }
      formOptions[key] = value;
      }
    }

  }

void HttpRequest::parseHeaders(BufferedReader& bf) {
  while (!bf.pastEnd()) {
    fb::String headerKey = bf.nextWord();
    fb::String headerValue = bf.nextWord();

    // strip trailing colon on key and  append to map
    headers[headerKey.substr(0, headerKey.size() - 1)] = headerValue;
  }
}
