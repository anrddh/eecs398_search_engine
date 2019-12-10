#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("frontend/title.html");
    return page;
}

int resultCounter = -1;
fb::UnorderedMap<fb::String, fb::String> resultOptions;

HtmlPage kthResults() {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    /* Invoke ranker */

    fb::SizeT random = rand();
    page.setValue("query", resultOptions["query"]);

    fb::String counterString = fb::toString(resultCounter);

    int numResults = 10;
    for(int i = 0; i < numResults; ++i) {
        fb::String iString = fb::toString(i);
        page.setValue("title" + iString, counterString +  iString + "th Result for " + resultOptions["query"]);
        page.setValue("url" + iString, counterString + iString + "th Url");
        page.setValue("snippet" + iString, counterString + iString + "th Snippet");
    }

    return page;
}

HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    resultCounter = 0;
    resultOptions = formOptions;
    return kthResults();
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

HtmlPage next( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter == -1 )
        return defaultPath();
    ++resultCounter;
    return kthResults();
}

HtmlPage previous( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter <= 0 )
        return defaultPath();
    --resultCounter;
    return kthResults();
}

int main() {
    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}
