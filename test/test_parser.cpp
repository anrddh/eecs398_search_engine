#include <parse/parser.hpp>

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

//   constexpr bool Parser::addUrls = false;
void printFlag( const uint8_t flag )
{
   if ( flag & INDEX_WORD_TITLE )
      std::cout << "title ";
   if ( flag & INDEX_WORD_BOLD )
      std::cout << "bold ";
   if ( flag & INDEX_WORD_HEADER )
      std::cout << "header ";
   std::cout << std::endl;
}


int main (int, char **argv){

   // //ParsedUrl in_url(argv[1]);

   // std::vector<String> urls;

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
   String filename = String(argv[1]);
   std::ifstream ifs(filename.data( ));
   std::string contentstd( (std::istreambuf_iterator<char>(ifs) ),
              (std::istreambuf_iterator<char>()    ) );
   String content( contentstd.c_str( ) );
   ifs.close();

   String domain = "https://en.wikipedia.org/wiki/Commutative_algebra";

   {
   bool addUrls = false;
   auto parser = Parser(content, filename);

   parser.parse();
   auto retstr = parser.getParsedResult();

   parser.printUrls();

   std::ofstream outfile;
   // outfile.open("htmlstagless/" + filename.substr(6));
   outfile.open(  (filename + "_tagless_not_add_urls").data() );

   // std::cout << retstr << std::endl;

   outfile << retstr;
   outfile.close();

   std::cout << parser.wordFlags.size() << std::endl;
   // for( const auto i : parser.wordFlags )
      // printFlag(i);
   }

   {
   bool addUrls = true;
   auto parser = Parser(content, filename);

   parser.parse();
   auto retstr = parser.getParsedResult();

   parser.printUrls();

   std::ofstream outfile;
   // outfile.open("htmlstagless/" + filename.substr(6));
   outfile.open(  (filename + "_tagless_add_urls").data() );

   // std::cout << retstr << std::endl;

   outfile << retstr;
   outfile.close();

   std::cout << parser.wordFlags.size() << std::endl;
   // for( const auto i : parser.wordFlags )
   //    printFlag(i);
   }

}
