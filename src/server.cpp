#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/algorithm.hpp>
#include <parse/query_parser.hpp>
#include <parse/tokenstream.hpp>

#include <parse/parser.hpp>
#include <parse/query_parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>
#include <ranker/ranker.hpp>

#include <query/page_result.hpp>

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

// Written by Jaeyoon Kim
fb::Mutex sockMtx;
AddrInfo masterLoc;

void parseArguments( int argc, char **argv )
   {
    fb::AutoLock l( sockMtx );
	option long_opts[ ] =
      {
		{ "hostname",  required_argument, nullptr, 'o' },
		{ "port",      required_argument, nullptr, 'p' },
		{ nullptr, 0, nullptr, 0 }
		};

	int option_idx;
	auto choice = 0;

	fb::String hostname, port, threads;

	while ( ( choice = getopt_long( argc, argv, "o:p:ht:", long_opts, &option_idx ) )
				 != -1 )
      {
		switch (choice)
         {
   		case 'p':
				port = optarg;
				break;
   		case 'o':
				hostname = optarg;
				std::cerr << hostname << '\n';
				break;
   		case 'h':
   		default:
            std::cerr << "Invalid option!" << std::endl;
            exit(1);
   		}
   	}

    AddrInfo addr(
			hostname.empty( ) ? DefaultHostname : hostname.data( ),
			port.empty( ) ? DefaultPort : port.data( ) );
    masterLoc = std::move( addr );
    }

fb::FileDesc open_socket_to_master() {
   auto sock = masterLoc.getConnectedSocket();

   // Finished establishing socket
   // Send verfication message
   send_int(sock, VERFICATION_CODE);

   return sock;
}

fb::Vector<PageResult> ask_query( fb::String query ) {
    fb::AutoLock l( sockMtx );
    try {
        fb::FileDesc sock = open_socket_to_master();
        send_str( sock, query );
        
        int num = recv_int( sock );
        fb::Vector<PageResult> results;
        for ( int i = 0; i < num; ++i ) {
            PageResult pr;
            pr.Url = recv_str( sock );
            pr.Title = recv_str( sock );
            pr.Snippet = recv_str( sock );
            pr.rank = recv_double( sock );
            results.pushBack( std::move( pr ) );
        }

        return results;
    } catch ( SocketException& se ) {
        // Failed to get 
        std::cerr << "Socket failed in ask_query" << std::endl;
        return {};
    }
}

// below written by Jinsoo Ihm

HtmlPage home( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    HtmlPage page;
    page.loadFromFile("frontend/title.html");
    return page;
}

int resultCounter = -1;
fb::UnorderedMap<fb::String, fb::String> resultOptions;
fb::Vector<PageResult> queryResult;

fb::String cleanedQuery( fb::String q )
{
    fb::String result = q;
    for (auto &c : result)
        if (CharIsIrrelevant(c))
           c = ' ';
    return result;
}

HtmlPage kthResults() {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    page.setValue("query", cleanedQuery(resultOptions["query"]));

    fb::SizeT numResults = 10;
    fb::SizeT numShow = fb::min(numResults, queryResult.size() - resultCounter * numResults);

    for ( int i = 0; i < numShow; ++i )
    {
        int index = queryResult.size( ) - resultCounter * numResults - i - 1;
        page.setValue("title" + fb::toString(i),
                      fb::String(queryResult[index].Title.data(), queryResult[index].Title.size()));
        page.setValue("url" + fb::toString(i),
                      fb::String(queryResult[index].Url.data(), queryResult[index].Url.size()));
        page.setValue("snippet" + fb::toString(i),
                      fb::String(queryResult[index].Snippet.data(), queryResult[index].Snippet.size()));
    }
    return page;
}

HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    resultCounter = 0;
    resultOptions = formOptions;
    queryResult = ask_query( formOptions["query"] );
    return kthResults();
}

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

HtmlPage pageNotFound( fb::String msg ) {
    HtmlPage page;
    page.loadFromFile("frontend/page_not_found.html");
    page.setValue("msg", msg);
}

HtmlPage next( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter == -1 )
        return defaultPath();
    ++resultCounter;
    return kthResults();
}

HtmlPage previous( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter <= 0 )
        return pageNotFound("You cannot press previous there");
    --resultCounter;
    return kthResults();
}



int main( int argc, char **argv ) {
    parseArguments( argc, argv );
    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}

