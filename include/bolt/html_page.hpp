#pragma once

#include "fb/memory.hpp"
#include "fb/string.hpp"
#include "fb/unordered_map.hpp"

enum HttpStatus : int
   {
   OK = 200,
   BAD_REQUEST = 400,
   FORBIDDEN = 403,
   NOT_FOUND = 404,
   INTERNAL_SERVER_ERROR = 500
   };

struct HttpHeader 
   {
   HttpStatus status = OK;
   fb::String ctype = "text/html";
   };

class HtmlTemplate;

class HtmlPage 
   {
public:
   HtmlPage();
   HtmlPage(const HtmlPage& other);
   HtmlPage& operator=(const HtmlPage& other);
   ~HtmlPage();

   void loadRawFile(const fb::String &path);
   void loadFromString(const fb::String &html);
   void loadFromFile(const fb::String &path);


   void setValue(const fb::String &key, const fb::String &value);
   
   fb::String getPageHtml();

   HttpHeader header;  // TEMP

private:
   fb::UnorderedMap<fb::String, fb::String> templateVariables;
   fb::UniquePtr<HtmlTemplate> htmlTemplate;
   };
