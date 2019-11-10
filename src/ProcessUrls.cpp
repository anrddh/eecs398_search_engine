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
#include "../lib/string.hpp"
#include "../lib/vector.hpp"
#include "../lib/url_pool.hpp"

using namespace fb;
using URL = String;


int ProcessURLs(UrlPool* Pool){
    //Receive the URLS to process from the URL pool
    Vector<URL> URLsToProcess = Pool->RequestUrls();
    for (auto currentURL : URLsToProcess){
        //TODO: all the stuff described above
    }

    return 0;
}
