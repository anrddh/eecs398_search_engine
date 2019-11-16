#include "master_url_tcp.hpp"
#include "url_tcp.hpp"
#include "vector.hpp"
#include <iostream>

using namespace fb;

char check_socket( int sock ) {
   if (recv_int(sock) != VERFICATION_CODE ) {
      throw SocketException("Incorrect verfication code!");
   }

   char message_type;
   if ( recv(sock , &message_type, sizeof(message_type) , MSG_WAITALL ) == -1) {
      throw SocketException("failed to get message type");
   }

   if (message_type != 'R' && message_type != 'S') {
      throw SocketException("Invalid message type");
   }

   return message_type;
}

void send_urls(int sock, const Vector<SizeT>& urls_to_parse) {
   std::cout << "in send urls 0" << std::endl;
   send_int(sock, urls_to_parse.size());
   std::cout << "in send urls 1" << std::endl;

   for (int i = 0; i < urls_to_parse.size(); ++i) {
      send_uint64_t(sock, urls_to_parse[i]);
      send_str(sock, "dummy string"); // TODO figure out how to get strings
   }
   std::cout << "in send urls 2" << std::endl;
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
