#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <parse/query_parser.hpp>

#include <parse/parser.hpp>
#include <parse/query_parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>
#include <ranker/ranker.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <iostream>
#include <fstream>
#include <cstdio>

#include <getopt.h>

#include <fb/vector.hpp>
#include <fb/string.hpp>

#include <query/query_result.hpp>

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
    page.setValue("title", "Google");
    page.setValue("url", "https://www.google.com");
    page.setValue("snippet", "Google snippet");

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
