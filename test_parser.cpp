#include "parser.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>

#include <iostream>
#include <fstream>
#include <cassert>

using namespace fb;

int main ( int argc, char *argv[] ){

   // //ParsedUrl in_url(argv[1]);

   // std::vector<std::string> urls;

   // //extractURL(GetHTTPS(argv[1]), urls);

   // ParsedUrl testguy("/relative/path");
   // std::cout << testguy << "\n\n\n\n\n";


   // ParsedUrl testguy2("http://google.com:304/file");
   // std::cout << testguy2 << "\n\n\n\n\n";

   // ParsedUrl testguy3("http://google.com/file");
   // std::cout << testguy3 << "\n\n\n\n\n";

   // ParsedUrl testguy4("http://file/sd");
   // std::cout << testguy4 << "\n\n\n\n\n";


   // for (auto i : urls){
   //    //std::cout << i << std::endl;
   // }
   std::string filename = std::string(argv[1]);
   std::ifstream ifs(filename);
   std::string content( (std::istreambuf_iterator<char>(ifs) ),
              (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   auto parser = Parser(content);

   auto retstr = parser.parse();

   for(auto i : parser.urlAnchorText)
   {
      std::cout << "URL is: " << i.first << std::endl;
      std::cout << "Anchor text:" << std::endl;
      for(auto j : i.second)
         std::cout << "\t" << j << std::endl;
   }

   std::ofstream outfile;
   outfile.open(filename += "tagless");

   outfile << retstr;
   outfile.close();

}
