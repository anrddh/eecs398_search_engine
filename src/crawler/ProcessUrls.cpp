//Created 11/9/19 by Chandler Meyers
/*This contains the functionality for processing URLs from the URL pool.
 *First, you ask for a list of URLS to process from the URL pool.
 *Given these URLs, you will send them to the parser and recieve from it the
 *information which you need about the content of the page to continue processing.
 *This information includes a list of outgoing links (in the form of URL, anchor text pairs)
 *and the location in hard memory of the downloaded page. It is now your responsibility
 *to look up the URL info struct, then update its anchor text, adjacency list,
 *and page reference. If one of the discovered URLs has not been processed before,
 *(which you will be able to tell from looking up its URL info), you must now
 *add it back to the frontier.
 */
#include "UrlStore.hpp"
#include "../../lib/string.hpp"
#include "../../lib/vector.hpp"
#include "url_pool.hpp"

using namespace fb;
using URL = String;

struct ParsedPageInfo {
    Vector<Pair<URL, String>> OutGoingLinks; //the outgoing links found on the page
    (uint *) Page; //the reference to the saved page //TODO: type
}; //TODO: this probably belongs in a different file, and maybe in a different form

int ProcessURLs(UrlPool* Pool){
    //Receive the URLS and offsets to process from the URL pool
    Vector<Pair<URL, SizeT>> URLsToProcess = Pool->RequestUrls();
    for (auto currentURL : URLsToProcess){
        //Send and recieve the info from the parser
        ParsedPageInfo parsedPage; // = parser->parse(currentURL);
        //Lookup the URL info struct
        Pool->
    }

    //Probably want to use return value to describe success/failure
    return 0;
}
