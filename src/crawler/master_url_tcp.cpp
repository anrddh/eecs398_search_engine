// Created by Jaeyoon Kim 11/15/2019
#include "master_url_tcp.hpp"
#include "url_tcp.hpp"
#include "UrlTables.hpp"
#include "../../lib/vector.hpp"
#include <iostream>

using namespace fb;

char get_message(type) {
   char message_type;
   if ( recv(sock , &message_type, sizeof(message_type) , MSG_WAITALL ) <= 0) {
      throw SocketException("failed to get message type");
   }
   return message_type;
}

void send_urls(int sock, const Vector<SizeT>& urls_to_parse) {
   send_int(sock, urls_to_parse.size());

   for (int i = 0; i < urls_to_parse.size(); ++i) {
      send_uint64_t(sock, urls_to_parse[i]);
      send_str(sock, UrlOffsetTable::getOffset().accessOffset( urls_to_parse[ i ]);
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
         String anchor_text = recv_str( sock );
         pp.links.pushBack(make_pair( std::move(link), std::move(anchor_text) ) );
      }
      recv_pages.pushBack( std::move(pp) );
   }

   return recv_pages;
}
