// #include <string>

#include <fstream>

#include <parse/parser.hpp>
#include <http/download_html.hpp>
#include <fb/string.hpp>
#include <fb/queue.hpp>
#include <fb/unordered_set.hpp>

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
   std::cout << "sadsa" << std::endl;
    fb::Queue<fb::String> urls;
    fb::UnorderedSet<fb::String> visited_urls;
    urls.push(fb::String( "https://web.eecs.umich.edu/~harshavm/eecs482/" ));
    std::cout << "sadsa" << std::endl;
    while(!urls.empty())
     {
     fb::String url = urls.front();
     urls.pop();
     if(visited_urls.find(url) != visited_urls.end())
             continue;

     std::cout << "Parsed URL is: " << url << std::endl;
     if(url.size() < 7)
             continue;

     std::cout << "\t Downloading " << url << std::endl;
        fb::String content;
     fb::String filename = "htmls_eecs482/";

     
     fb::String result;
      

     for(int i = 0; i < url.size(); ++i)
         {
         if(isalnum(url[i]))
             filename += url[i];
         }
     filename += ".html";

     std::cout << "Before downloading" << std::endl;
     try
         {
         auto ht = HTTPDownloader();
         result = ht.PrintHtml( url );
         }
     catch (...)
         {
         std::cerr << "\t Failed at link" << std::endl;
         continue;
         }
      std::cout << "Done downloading" << std::endl;

      visited_urls.insert(url);
      writeVisitedUrls(url);
      saveString( result, filename );

      auto pa = ParsedUrl(url);
      fb::String domain = pa.Service + "://" + pa.Host;

      fb::Parser parser( result,  domain );
      parser.parse();

      // std::cout << "Found urls" << std::endl;
      // std::cout << "domain: " << domain << std::endl;
      for ( auto i = parser.urlAnchorText.begin(); i != parser.urlAnchorText.end(); ++i )
         urls.push(i.key());

      saveString( parser.getParsedResult(), filename + "_tagless");
      }

   //  fb::String url = argv[ 1 ];
   // std::cout << "Try downloading html file" << std::endl;
   // auto ht = HTTPDownloader();
   // try 
   //    {
   //    auto a = ht.PrintHtml( url );
   //    std::cout << a << std::endl;
   //    }
   // catch ( ConnectionException e)
   //    {
   //    std::cout << "Exception caught:  " << e.msg << std::endl;
   //    }

   // std::cout << "Try downloading txt file" << std::endl;
   // auto htt = HTTPDownloader();
   // try 
   //    {
   //    auto a = htt.PrintPlainTxt( url );
   //    std::cout << a << std::endl;
   //    }
   // catch ( ConnectionException e)
   //    {
   //    std::cout << "Exception caught:  " << e.msg << std::endl;
   //    }

    }
