#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <deque>

#include "parser.hpp"
#include "DownloadHTML.hpp"

void writeVisitedUrls(std::string url)
{
  std::ofstream outfile;

  outfile.open("visted_urls.txt", std::ios_base::app);
  outfile << url << "\n"; 
}

int main( int argc, char *argv[ ] ) {
    std::deque<std::string> urls;
    std::set<std::string> visited_urls;
    urls.push_back(argv[1]);

    while(!urls.empty())
    {
        std::string url = urls.front();
        urls.pop_front();
        if(visited_urls.find(url) != visited_urls.end())
            continue;

        std::cout << "Parsed URL is: " << url << std::endl;
        if(url.length() < 7)
            continue;
        // std::string https = "https://";
        // if(url.substr(0, https.length()) != https)
        //     continue;

        std::cout << "\t Downloading " << url << std::endl;
        std::string filename = "htmls/";
        for(int i = 0; i < url.length(); ++i)
        {
            if(isalnum(url[i]))
                filename += url[i];
        }
        filename += ".html";
        try
            {
            PrintHtml( url, filename );
            }
        catch (...)
            {
            std::cerr << "\t Failed at link" << std::endl;
            continue;
            }

        visited_urls.insert(url);

        writeVisitedUrls(url);

        std::ifstream ifs(filename);
        std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

        auto pa = ParsedUrl(url);
        std::string domain = pa.Service + "://" + pa.Host;
        fb::extractURL(content, urls, domain);
   }


    // std::string url = argv[ 1 ];
    // PrintHtml( url, "downloaded_html.html" );
}
