//#pragma once

#include<vector>
#include<string>
#include "GetSSL.hpp"

namespace fb
{

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
