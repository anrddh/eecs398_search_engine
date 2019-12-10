#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("../html/index.html");
    return page;
}

HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromString("<h1>Cars [% car %]!</h1><p>User Query=[% query %]</p>");

    /* Invoke ranker */

    fb::SizeT random = rand();
    page.setValue("query", formOptions["query"]);

    return page;
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

int main() {
    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);

    bolt.run();
}
