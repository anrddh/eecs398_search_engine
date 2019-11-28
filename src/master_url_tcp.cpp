// Created by Jaeyoon Kim 11/15/2019
#include "tcp/master_url_tcp.hpp"
#include "tcp/url_tcp.hpp"
#include "disk/frontier.hpp"
#include "disk/url_store.hpp"
#include "fb/vector.hpp"
#include <iostream>

using namespace fb;

void send_urls(int sock, const Vector<SizeT>& urls_to_parse) {
   send_int(sock, urls_to_parse.size());

   for (SizeT i = 0; i < urls_to_parse.size(); ++i) {
      send_uint64_t(sock, urls_to_parse[i]);
      send_str(sock, UrlStore::getStore().getUrl( urls_to_parse[ i ] ) );
   }
}

Vector<ParsedPage> recv_parsed_pages(int sock) {
   Vector<ParsedPage> recv_pages;
   int num_pages = recv_int(sock);
   int num_links;
   for (int i = 0; i < num_pages; ++i) {
      ParsedPage pp;
      pp.url_offset = recv_uint64_t( sock );
      num_links = recv_int( sock );
      for (int j = 0; j < num_links; ++j ) {
         String link = recv_str( sock );
         pp.links.emplaceBack( std::move(link) );
      }
      recv_pages.pushBack( std::move(pp) );
   }

   return recv_pages;
}
