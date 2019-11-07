#include "GetSSL.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>

#include <iostream>
#include <cassert>

using namespace fb;

int main ( int argc, char *argv[] ){

   //ParsedUrl in_url(argv[1]);

   std::vector<std::string> urls;

   //extractURL(GetHTTPS(argv[1]), urls);

   ParsedUrl testguy("/relative/path");
   std::cout << testguy << "\n\n\n\n\n";


   ParsedUrl testguy2("http://google.com:304/file");
   std::cout << testguy2 << "\n\n\n\n\n";

   ParsedUrl testguy3("http://google.com/file");
   std::cout << testguy3 << "\n\n\n\n\n";

   ParsedUrl testguy4("http://file/sd");
   std::cout << testguy4 << "\n\n\n\n\n";


   for (auto i : urls){
      //std::cout << i << std::endl;
   }
}
