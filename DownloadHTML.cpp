#include <string>

#include "DownloadHTML.hpp"

int main( int argc, char *argv[ ] ) {
   std::string url = argv[ 1 ];
   PrintHtml( url, "downloaded_html.html" );
}
