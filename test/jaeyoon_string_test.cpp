#include <fb/string.hpp>
#include <iostream>

using std::cout;
using std::endl;

using namespace fb;

int main() {
    String urlStr = "https://test_url";
    String titleStr = "Some good title ";
    String snippetStr = "blah blah blah ";
    for (int i = 0; i < 100; ++i ) {
        PageResult pr;
        String val = toString( ++n );
        pr.Url = urlStr + val;
        pr.Title = titleStr + val;
        pr.Snippet = snippetStr + val;
        pr.rank = n;
        std::cout << "got url " << pr.Url << std::endl;
        topPages.push( std::move( pr ) );
    }

}
