//#pragma once

#include<vector>
#include<string>
#include "GetSSL.hpp"

namespace fb
{

struct View {
    const char *p = nullptr;
    std::size_t s = 0;
};

std::ostream& operator<<(std::ostream &os, View &v) {
    for (size_t i = 0; i < v.s; ++i) {
        os << v.p[i];
    }

    return os;
}

class ParsedUrl
   {
   public:
      const std::string url;

      View Service, Host, Port, Path;
       bool success = false;

      ParsedUrl( std::string url_ ) : url(std::move(url_))
         {
         Service.p = url.c_str();
         const char Colon = ':', Slash = '/';

         std::size_t p = url.find(':');
         if (p == std::string::npos || !p) {
            Path.p = url.c_str();
            Path.s = url.size();
            success = true;
            return;
         }

         Service.s = p++;

         /*
           TODO: If Service does not belong to allowed_services, we exit.
          */

         if ( url[p] == Slash )
             p++;
         if ( url[p] == Slash )
             p++;

         Host.p = url.c_str() + p;

         // check for stl algorithms
         for ( ; p < url.size() && url[p] != Slash && url[p] != Colon; ++p )
             ;

         Host.s = url.c_str() + p - Host.p;

         if (p < url.size() && url[p] == Colon) {
           // Port specified.  Skip over the colon and
           // the port number.
             ++p;
             Port.p = url.c_str() + p;
             for (; p < url.size() && url[p] != Slash; ++p)
                 ;
             Port.s = url.c_str() + p - Port.p;
         }

         Path.p = url.c_str() + p;
         Path.s = url.size() - p;
         success = true;
      }

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
      constexpr static std::array<const char *, 2> allowed_services = {"https", "http"};
   };

   std::ostream& operator << ( std::ostream& os, ParsedUrl &testguy )
      {
      os << "complete: " << testguy.url << std::endl
         << "service: " << testguy.Service << std::endl
         << "host: " << testguy.Host << std::endl
         << "port: " << testguy.Port << std::endl
         << "path: " << testguy.Path << std::endl;
      return os;
      }


// take in string and a container, vector for now, and append newly found urls
// to the given container.
void extractURL( const std::string & line, std::vector< std::string > & urls )
    {
    std::size_t found = line.find( "href" );
    while( found != std::string::npos )
        {
        // There might be space between href and =
        found = line.find( "\"", found );
        if ( found == std::string::npos )
            return;

        // found + 1 to find the next closing quote
        // if no closing quote is found, skip.
        std::size_t endURL = line.find( "\"", found + 1 );
        if( endURL == std::string::npos )
            return;

        std::string url = line.substr( found + 1, endURL - found - 1);

        // just in case a closing quote is not really closing the url, one heuristic is
        // that there would be a space somewhere.
        // For example, we might have
        // href="https://www.nytimes.com/es/ href =    "https://www.nytimes.com/es/
        // it mighbe be possible to just add the substring until the space, but we will see.
        if( url.find( " " ) == std::string::npos )
           {
            ParsedUrl parsedUrl( url.c_str( ) );
            urls.push_back( url );
            }

        found = line.find( "href", endURL );
        }
    }
};
