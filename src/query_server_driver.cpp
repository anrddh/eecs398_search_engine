// Written by Jaeyoon Kim added 12/10
//
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <query/query_result.hpp>
#include <query/page_result.hpp>

#include <parse/query_parser.hpp>

#include <disk/page_store.hpp>
#include <disk/url_store.hpp>
#include <disk/UrlInfo.hpp>
#include <disk/constants.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>
#include <fb/unordered_set.hpp>

#include <bolt/bolt.hpp>

#include <debug.hpp>

#include <iostream>
#include <fstream>

#include <getopt.h>
#include <cctype>
#include <sstream>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

using namespace fb;

constexpr int MAX_NUM_PAGES = 100; // Number of pages sent back to front end server per query
static_assert( MAX_NUM_PAGES > 0 );

constexpr int TCP_TIMEOUT_LIMIT = 30; // Number of seconds before timeout (only applies to talking to workers)

// The value we scale the ranking based on url
constexpr double urlWeight = 0.0001; // STATIC
constexpr double title_match = 0.1;
constexpr double url_match = 0.25;

// Server when accepting:
// verfication code (int)
// char Q (query)
// or char W (new worker)
constexpr char queryMessageType = 'Q';
constexpr char workerMessageType = 'W';

FileDesc parseArguments(int argc, char **argv);

//Hash instance for PageResult
template <>
struct fb::Hash<FileDesc> {
    // Jin Soo removed constexpr
    SizeT operator() ( const FileDesc& data ) const noexcept {
        return fnvHash( (char *) &data, sizeof(data) );
    }
};

Mutex socketsMtx;
UnorderedSet<FileDesc> socketsToWorkers;

struct WorkerArg {
    FileDesc* sock;
    StringView query;
    TopNQueue<PageResult>* topPages;
};

// TODO this is duplicated in worker driver
struct ArgError : std::exception
   {
   };

// If this is a querry, we do not return until the query is completed
// Closes sockets if necessary
//
// Takes a dynamically allocated Worker arg
// this function deletes the pointer
// returns nullptr
void* ask_workers(void* );
QueryResult recv_query_result( int sock );
void handle_new_worker( FileDesc&& sock );

void* accept_workers(void* ptr) {
    FileDesc* server_fd = (FileDesc*) ptr;
    while ( true ) {
        // Do not use FileDesc!
        // handle_connection will close sockets
        try {
            FileDesc sock = accept(*server_fd, nullptr, nullptr);
            handle_new_worker( std::move( sock ) );
        } catch (SocketException& se) {
            cout << se.what() << endl;

        } catch (...) {
            perror("failed in accept"); 
            throw;
        }
    }
}

void handle_new_worker( FileDesc&& sock ) {
    if (recv_int(sock) != VERFICATION_CODE) {
        // We just ignore it!
        return;
    }

    if (recv_char( sock ) != 'W') {
        return;
    }

    std::cout << "got new worker!" << std::endl;

    int set = 1;
    setsockopt(sock, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));
    struct timeval timeout;
    timeout.tv_sec = TCP_TIMEOUT_LIMIT;
    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        std::cerr << "failed at setsockopt recv" << std::endl;

    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
            sizeof(timeout)) < 0)
        std::cerr << "failed at setsockopt send" << std::endl;

    AutoLock l( socketsMtx );
    bool success = socketsToWorkers.insert( std::move( sock ) );
    assert( success );
}

fb::String lower_string(const fb::String& str) {
    fb::String new_str;

    for(auto c : str)
    {
        new_str += tolower( c );
    }
    return new_str;
}

void* ask_workers( void* worker_query ) {
    WorkerArg arg = * ( WorkerArg* ) worker_query; // copy
    delete (WorkerArg*) worker_query;
    fb::String new_string;
    for(auto c : arg.query)
    {
        if( isalnum(c) )
            new_string += tolower(c);
        else
            new_string += " ";
    }
    std::stringstream ss( new_string.data() );
    std::string word;
    fb::Vector<fb::String> words;
    while (ss >> word) {
        words.pushBack(word.c_str());
    }


    try {
        send_str( *arg.sock, arg.query );
        int numPages = recv_int( *arg.sock );
        std::cout << "received " << numPages << " many pages " << std::endl;
        for ( int i = 0; i < numPages; ++i ) {
            PageResult pr;
            pr.Url += UrlStore::getStore().getUrl( recv_uint64_t( *arg.sock ) );
            pr.Title = recv_str( *arg.sock );
            pr.Snippet = recv_str( *arg.sock );
            pr.rank = recv_double( *arg.sock );

            double dynamic_title_rank = 0;
            double dynamic_url_rank = 0;
            fb::String lower_title = lower_string( pr.Title );
            fb::String lower_url = lower_string( pr.Url );
            for (auto& word : words) {
                if ( lower_url.find(word) != fb::String::npos ) {
                    dynamic_url_rank += url_match;
                }
                if ( lower_title.find(word) != fb::String::npos ) {
                    dynamic_title_rank += title_match;
                }
            }

            if (lower_title.size() != 0) {
		dynamic_title_rank *= 100;
                dynamic_title_rank /= lower_title.size();
            }


	    /*
            // TODO delete
            std::cout <<  pr.Url << std::endl; // TODO delete this
            std::cout << "\ttitle " << pr.Title  << std::endl; // TODO delete this
            std::cout << "\ttfidf ranking: " << pr.rank << " dynamic url ranking " << dynamic_url_rank << " static url " << urlWeight * RankUrl( pr.Url ) 
		    << " title rank " << dynamic_title_rank << std::endl; // TODO delete this
		*/

            pr.rank += urlWeight * RankUrl( pr.Url );
            pr.rank += dynamic_title_rank;
            pr.rank += dynamic_url_rank;

            arg.topPages->push( std::move( pr ) );
        }

    } catch ( SocketException& se ) {
        // Remove this socket from list of valid sockets!
        std::cout << "error" << se.what() << std::endl;
        AutoLock l( socketsMtx );
        socketsToWorkers.erase( *arg.sock );
    }

    return nullptr;
}

FileDesc parseArguments( int argc, char **argv ) {
    try {
        option long_opts[] = {
           {"port",     required_argument, nullptr, 'p'},
           {"help",     no_argument,       nullptr, 'h'},
           {nullptr, 0, nullptr, 0}
        };
        opterr = true;

        int option_idx;
        auto choice = 0;

        String port;
        while ((choice =
               getopt_long(argc, argv, "p:h", long_opts, &option_idx))
              != -1) {
           switch (choice) {
           case 'p':
               port = optarg;
               break;
           case 'h':
           default:
               throw ArgError();
           }
        }

       auto rootDir = getRootDir();
       auto urlstoreloc = rootDir + UrlStoreFile;
       UrlStore::init(urlstoreloc);

        AddrInfo info(nullptr, port.empty() ? DefaultPort : port.data());
        return info.getBoundSocket();
    } catch( const SocketException& se) {
        cerr << se.what() << '\n';
        throw ArgError();
    } catch ( const FileDesc::ConstructionError &e ) {
        cerr << e.what() << '\n';
        throw ArgError();
    }
}

// Written by Jaeyoon Kim
fb::Vector<PageResult> ask_query( fb::String query ) {
    QueryParser qp( query );
    if (qp.Parse().get() == nullptr) {
	cout << query << " is an invalid query " << endl;
        return {};
    }

    Vector<Thread> threads;
    TopNQueue<PageResult> topPages( MAX_NUM_PAGES );
    std::cout << "got new query! " << query << std::endl;

    // TODO handle exceptions

    socketsMtx.lock();
    for ( auto it = socketsToWorkers.begin(); it != socketsToWorkers.end(); ++it ) {
        threads.emplaceBack( ask_workers, new WorkerArg{ &*it, query, &topPages } );
    }
    socketsMtx.unlock();

    for ( Thread& t : threads ) {
        t.join();
    }
    cout << "joined all workers" << endl;

    fb::Vector<PageResult> results;
    while ( !topPages.empty() ) {
        results.pushBack( topPages.top() );
        topPages.pop();
    }

    return results;
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
HtmlPage pageNotFound( fb::String msg );

fb::String specialCharacter(fb::String encoding)
{
    if(encoding == "%26")
	    return " ";
    else if(encoding == "%7C")
	    return "|";
    else if(encoding == "%5C")
	    return "\\";
    else if(encoding == "%28")
	    return "(";
    else if(encoding == "%29")
	    return ")";
    else if(encoding == "%22")
	    return "\"";
    else if(encoding == "%60")
        return "`";
    else if(encoding == "%27")
        return "\'";
    else
	    return "";
}

fb::String cleanedQuery( fb::String q )
{
    fb::String result;
    for (int i = 0; i < q.size(); ++i) {
	if (q[i] == '%')
	{
	    result += specialCharacter(q.substr(i, 3));
	    i += 2;
	}
	else if (CharIsIrrelevant(q[i]))
           result += ' ';
	else
	    result += q[i];
    }
    return result;
}

HtmlPage kthResults() {
    HtmlPage page;
    page.loadFromFile("frontend/search_results.html");

    page.setValue("query", cleanedQuery(resultOptions["query"]));

    fb::SizeT numResults = 10;
    fb::SizeT numShow = 0;
    if ( queryResult.size() > resultCounter * numResults )
        numShow = fb::min(numResults, queryResult.size() - resultCounter * numResults);
    else
        return pageNotFound("No more results to show.");

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

HtmlPage defaultPath() {
    HtmlPage page;
    page.loadFromString("<h1>Page not found.</h1>");
    return page;
}

HtmlPage pageNotFound( fb::String msg ) {
    HtmlPage page;
    page.loadFromFile("frontend/page_not_found.html");
    page.setValue("msg", msg);
    page.setValue("query", cleanedQuery(resultOptions["query"])); 
    return page;
}


HtmlPage results( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
	std::cout << "Results start" << std::endl;
    resultCounter = 0;
    std::cout << "here" << std::endl;
    
    resultOptions = formOptions;
    std::cout << "hereee" << std::endl;
    if( formOptions["query"].empty() )
        return pageNotFound( "You should type something!" );
    std::cout << "hereeeee" << std::endl;
    std::cout << formOptions["query"] << std::endl;
    auto s = cleanedQuery(formOptions["query"]);
    std::cout << formOptions["query"] << std::endl;
    std::cout << s << std::endl;
    queryResult = ask_query( s );
    return kthResults();
}

HtmlPage next( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter == -1 )
        return pageNotFound( "This should not happen..." );
    ++resultCounter;
    return kthResults();
}

HtmlPage previous( fb::UnorderedMap<fb::String, fb::String> formOptions ) {
    if( resultCounter <= 0 )
        return pageNotFound("You cannot press previous there!");
    --resultCounter;
    return kthResults();
}

int main( int argc, char **argv ) {
    FileDesc server_fd = parseArguments( argc, argv );
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    Thread worker_getter( accept_workers, &server_fd );
    worker_getter.detach();

    Bolt bolt;
    bolt.registerHandler("/", home);
    bolt.registerHandler("/results", results);
    bolt.registerHandler("/next", next);
    bolt.registerHandler("/previous", previous);

    bolt.run();
}
