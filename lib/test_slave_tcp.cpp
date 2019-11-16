#include "slave_url_tcp.hpp"
#include "string.hpp"
#include "thread.hpp"
#include <iostream>

using namespace fb;

void* dummy_parse(void*) {
   while(true) {
      std::cout << "in loop" << std::endl;
      try {
         get_url_to_parse();
         std::cout << "after to_parse" << std::endl;
         for (int i = 0; i < 2; ++i) {
            ParsedPage pp;
            pp.url_offset = i;
            pp.links.pushBack(make_pair<String, String>("url1", "anchor_text1"));
            pp.links.pushBack(make_pair<String, String>("url2", "anchor_text2"));
            add_parsed(std::move(pp));
         }
      } catch (SocketException& except) {
         std::cout << except.what() << std::endl;
      }
   }
}

int main(){
   int master_port = 8992; // HARD coding for testing
   String master_ip = "127.0.0.1";

   set_master_ip(master_ip, master_port);

   dummy_parse(nullptr);

   //Thread t1(dummy_parse, nullptr);
   /*
   Thread t2(dummy_parse, nullptr);
   Thread t3(dummy_parse, nullptr);
   Thread t4(dummy_parse, nullptr);
   Thread t5(dummy_parse, nullptr);
   Thread t6(dummy_parse, nullptr);
   */

   while (true);

   return 0;
}
