// #include <string>

#include <fstream>

#include "parser/parser.hpp"
#include "DownloadHTML.hpp"
#include "lib/string.hpp"
#include "lib/queue.hpp"
#include "lib/unordered_set.hpp"

void writeVisitedUrls(fb::String url)
{
	std::ofstream outfile;

	outfile.open("visted_urls.txt", std::ios_base::app);
	outfile << url << "\n"; 
}

fb::String readString( fb::String filename )
{
   std::ifstream ifs(filename.data());
   std::string content( (std::istreambuf_iterator<char>(ifs)),
		 (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   return fb::String(content.c_str());
}

void saveString( const fb::String &content, fb::String filename )
{
	std::ofstream outfile;
   outfile.open( filename.data() );
   outfile << content;
   outfile.close();
}

int main( int argc, char *argv[ ] ) 
	{
	// fb::Queue<fb::String> urls;
	// fb::UnorderedSet<fb::String> visited_urls;
	// urls.push(fb::String(argv[1]));

	// while(!urls.empty())
	// 	{
	// 	fb::String url = urls.front();
	// 	urls.pop();
	// 	if(visited_urls.find(url) != visited_urls.end())
	// 			continue;

	// 	std::cout << "Parsed URL is: " << url << std::endl;
	// 	if(url.size() < 7)
	// 			continue;

	// 	std::cout << "\t Downloading " << url << std::endl;
 //        fb::String content;
	// 	fb::String filename = "htmls_new/";
	// 	for(int i = 0; i < url.size(); ++i)
 //    		{
	// 		if(isalnum(url[i]))
	// 			filename += url[i];
 //    		}
	// 	filename += ".html";
	// 	try
	// 		{
	// 		PrintHtml( url, content );
	// 		}
	// 	catch (...)
	// 		{
	// 		std::cerr << "\t Failed at link" << std::endl;
	// 		continue;
	// 		}

	// 	visited_urls.insert(url);

	// 	writeVisitedUrls(url);

 //        saveString( content, filename );

	// 	auto pa = ParsedUrl(url);
	// 	fb::String domain = pa.Service + "://" + pa.Host;

	// 	fb::Parser parser( content,  domain );
	// 	parser.parse();

 //        // std::cout << "Found urls" << std::endl;
 //        // std::cout << "domain: " << domain << std::endl;
	// 	for ( auto i = parser.urlAnchorText.begin(); i != parser.urlAnchorText.end(); ++i )
	// 	{
 //            // std::cout << i.key() << std::endl;
	// 		urls.push(i.key());
	// 	}

 //        saveString( parser.getParsedResult(), filename + "_tagless");
 //    	}

		fb::String url = argv[ 1 ];
      fb::String downloadedContent;
      auto ht = HTTPDownloader();
      auto a = ht.PrintHtml( url );
      // std::cout << "print html" << std::endl;
      // std::cout << a << std::endl;
      std::cout << a.downloadedContent << std::endl;
      // std::cout << "response " << a.response << std::endl;

      // if ( a.response[0] == '4' )
      //   std::cout << ht.header << std::endl;

      // auto htt = HTTPDownloader();
      // auto b = htt.PrintPlainTxt( url );
      // std::cout << "print plain txt" << std::endl;
      // // std::cout << b << std::endl;
      // std::cout << b.downloadedContent << std::endl;
	}