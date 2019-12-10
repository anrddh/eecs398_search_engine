#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("frontend/title.html");
    return page;
}

HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    /* Invoke ranker */

    fb::SizeT random = rand();
    page.setValue("query", formOptions["query"]);

    int numResults = 10;
    for(int i = 0; i < numResults; ++i) {
        fb::String iString = fb::toString(i);
        page.setValue("title" + iString, iString + "th Result for " + formOptions["query"]);
        page.setValue("url" + iString, iString + "th Url");
        page.setValue("snippet" + iString, iString + "th Snippet");
    }

    return page;
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

HtmlPage next( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromString("<h1>Next page not found.</h1>");
    return page;
}

HtmlPage previous( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromString("<h1>Previous page not found.</h1>");
    return page;
}

int main() {
    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}
