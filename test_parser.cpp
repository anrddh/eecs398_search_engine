#include "parser.hpp"
#include "GetSSL.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <vector>

#include <iostream>
#include <cassert>

using namespace fb;

int main ( int argc, char *argv[] ){

   //ParsedUrl in_url(argv[1]);

   std::vector<std::string> urls;

   extractURL(GetHTTPS(argv[1]), urls);

   ParsedUrl testguy("/relative/path");
   std::cout << testguy;

   for (auto i : urls){
      //std::cout << i << std::endl;
   }
}
