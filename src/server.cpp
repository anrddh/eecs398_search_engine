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

#include "query_result.hpp"

fb::UniquePtr<Expression> ParseQuery( fb::String query )
    {
    // First: we send the query to the query compiler
    Parser p(query);
    return p.Parse( );
    }


Vector<String> GenerateSnippets( Vector<SnippetStats> Stats ){

    Vector<String> snippets;
    for(auto& stat : Stats ){
        FILE *fptr = fopen(stat.filename.data(), "rb");
        if (fptr == NULL){
            std::cout << "error opening " << filename << " when generating snippets " << std::endl;
            snippets.PushBack(""); //if the file fails to open, just give empty string for snippet rather than crash
            continue;
        }
        fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_SET); //skip the cursor
        fseek(fptr, sizeof(std::atomic<fb::SizeT>), SEEK_CUR); //skip the counter
        fseek(fptr, sizeof(PageHeader) * stat.DocIndex, SEEK_CUR); //skip ahead in the vector of PageHeaders
        fb::SizeT PageOffset;
        fread(&PageOffset, fb::SizeT, 1, fptr); //read in the page offset

        String snippet;
        char dummy[80]; //TODO: THIS IS SCARY!! do we have a max word size??
        fseek(fptr, PageOffset, SEEK_SET); //jump to that offset to begin reading the page
        for (int i = 0; i < stat.Offsets.begin; ++i){
            fscanf(fptr, "%s", dummy); //scan past all the words before begin offset
        }

        for (int j = stat.Offsets.begin; j < stat.Offsets.end; ++j){
            fscanf(fptr, "%s", dummy); //add all the words between begin offset and end offset
            snippet += dummy;
            snippet += " "; //dont forget to put a space between the words!
        }

        fclose(fptr); //don't forget this!
        snippets.PushBack(snippet); //add the generated snippet to vector
    }
    return snippets;
}




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
