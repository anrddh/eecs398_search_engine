#include "slave_url_tcp.hpp"
#include "string.hpp"
#include <iostream>

using namespace fb;

int main(){
   int master_port = 8992; // HARD coding for testing
   String master_ip = "127.0.0.1";

   set_master_ip(master_ip, master_port);

   String t;
   for (int j = 0; j < 100; ++j) {
      t += 'a';
   }

   std::cout << t << std::endl;

   try {
      for (int i = 0; i < 100; ++i) {
         ParsedPage pp;
         pp.url_offset = i;
         String url;
         for (int j = 0; j < i; ++j) {
            url += 'a';
         }
         pp.links.pushBack(make_pair<String, String>(std::move(url), "anchor_text"));
         add_parsed(std::move(pp));
      }
   } catch (SocketException& except) {
      std::cout << except.what() << std::endl;
   }
   return 0;
}
