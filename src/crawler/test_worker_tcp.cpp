#include "slave_url_tcp.hpp"
#include "string.hpp"
#include "thread.hpp"
#include <iostream>

using namespace fb;

void* dummy_parse(void* val) {
   int* num = (int*) val;
   while(true) {
      std::cout << "in loop" << *num << std::endl;
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

int main() {
   int master_port = 8992; // HARD coding for testing
   String master_ip = "127.0.0.1";

   set_master_ip(master_ip, master_port);

   Vector<Thread> threads;
   int ids[] = {1, 2, 3, 4, 5, 6};
   for (int i = 0; i < 6; ++i) {
      Thread t(dummy_parse, ids + i);
      threads.pushBack(std::move(t));
   }

   while (true);

   return 0;
}
