#include <bolt/bolt.hpp>

#include <fb/vector.hpp>
#include <fb/string.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/algorithm.hpp>
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
        send_char( sock, 'Q' ); // indicate its a query
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

HtmlPage kthResults() {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    /* Invoke ranker */

    // fb::SizeT random = rand();
    page.setValue("query", resultOptions["query"]);

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
    // fb::String counterString = fb::toString(resultCounter);

    // if( resultOptions["query"] == "fantastic+bugs" )
    // {
    //     page.setValue("title0", "Fanatstic Bugs Presentation");
    //     page.setValue("url0", "https://docs.google.com/presentation/d/1VGzVglZy6j6fE8RD95SxyeLF8HCY3NdpRwY29NMRGEw/edit#slide=id.p");
    //     page.setValue("snippet0", "");
    // }

    // else if( resultOptions["query"] == "aniruddh" )
    // {
    //     page.setValue("title0", "Aniruddh Agarwal - MATH 593 (Graduate Algebra I) Grader...");
    //     page.setValue("url0", "https://www.linkedin.com/in/agarwalaniruddh");
    //     page.setValue("snippet0", "Undergraduate student at the University of Michigan majoring in Computer Science and Mathematics. Interested in systems programming and machine learning.");

    //     page.setValue("title1", "User Aniruddh Agarwal - MathOverflow");
    //     page.setValue("url1", "https://mathoverflow.net/users/136734/aniruddh-agarwal");
    //     page.setValue("snippet1", "Undergrad at Michigan interested in arithmetic geometry.");

    //     page.setValue("title2", "Aniruddh Agarwal");
    //     page.setValue("url2", "https://www.facebook.com/aniruddh.agarwal.3");
    //     page.setValue("snippet2", "");

    //     page.setValue("title3", "Aniruddh Agarwal");
    //     page.setValue("url3", "https://github.com/anrddh");
    //     page.setValue("snippet3", "Aniruddh Agarwal anrddh");

    //     page.setValue("title4", "User Aniruddh Agarwal - Stack Overflow");
    //     page.setValue("url4", "https://stackoverflow.com/users/11428421/aniruddh-agarwal");
    //     page.setValue("snippet4", "Undergrad at Michigan interested in arithmetic geometry.");
    // }
    // else
    // {
    //     // int numResults = 10;
    //     // for(int i = 0; i < numResults; ++i) {
    //     //     fb::String iString = fb::toString(i);
    //     //     page.setValue("title" + iString, counterString +  iString + "th Result for " + resultOptions["query"]);
    //     //     page.setValue("url" + iString, counterString + iString + "th Url");
    //     //     page.setValue("snippet" + iString, counterString + iString + "th Snippet");
    //     // }
    //     page.setValue("title0", "Did you mean Nicole Hamilton?");
    //     page.setValue("url0" , "https://hamiltonlabs.com/");

    //     page.setValue("title1", "Hamilton C shell");
    //     page.setValue("url1", "https://en.wikipedia.org/wiki/Hamilton_C_shell");
    //     page.setValue("snippet1", "Hamilton C shell is a clone of the Unix C shell and utilities[1][2] for Microsoft Windows created by Nicole Hamilton[3] at Hamilton Laboratories as a completely original work, not based on any prior code. It was first released on OS/2 on December 12, 1988[4][5][6][7][8][9] and on Windows NT in July 1992.[10][11][12] The OS/2 version was discontinued in 2003 but the Windows version continues to be actively supported.");

    //     page.setValue("title2", "EECS 280: Programming and Intro Data Structures");
    //     page.setValue("url2", "https://eecs280staff.github.io/eecs280.org/");
    //     page.setValue("snippet2", "Computer science fundamentals, with programming in C++. Build an image processing program, a game of Euchre, a web backend, and a machine learning algorithm.");
    // }

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

int main( int argc, char **argv ) {
    parseArguments( argc, argv );
    auto pages = ask_query( "hi" );

    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}

