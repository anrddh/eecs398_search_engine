#include <parse/parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <tcp/addr_info.hpp>
#include <tcp/constants.hpp>
#include <disk/page_store.hpp>
#include <disk/constants.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>
#include <fb/shared_mutex.hpp>

#include <disk/logfile.hpp>
#include <debug.hpp>

#include <iostream>
#include <fstream>

#include <getopt.h>

constexpr fb::SizeT NumThreadsToSpawn = 50;

fb::Mutex endLock;
fb::CV endCV;

fb::SharedMutex blockedHostsLock;

using std::cout;
using std::endl;

bool addUrls = true;

struct ArgError : std::exception 
   {
   };

fb::SizeT parseArguments( int argc, char **argv );
void * parsePages( void * );

fb::UnorderedSet< fb::String > blockedHosts;

void addBlockedHosts( fb::String filename )
   {
   std::ifstream file;
   file.open( filename.data( ) );

   if( !file.is_open( ) )
      {
      std::cerr << "Could not open file `" << filename
               << "'." << std::endl;
      return;
      }

   blockedHostsLock.lock( );
   fb::String host;
   while ( fb::getline( file, host ) )
      blockedHosts.insert( host );
   blockedHostsLock.unlock( );

   file.close( );

   std::cout << "Number of blocked hosts: " << blockedHosts.size( ) << std::endl;
   }

int main( int argc, char **argv ) 
   try 
      {
   	auto numThreads = parseArguments( argc, argv );

   	SSLWrapper::SSLInit( );

   	fb::Vector< fb::Thread > threads;
   	threads.reserve( numThreads );

   	std::cout << "Invoking " << numThreads << " threads\n";
   	for ( fb::SizeT i = 0;  i < numThreads;  ++i )
      	threads.emplaceBack( parsePages, nullptr );

   	fb::String userInput;
   	do 
         {
   		if ( userInput == "status" || userInput == "s" )
   			{
   			print_tcp_status( );
   			std::cout << "Num pages parsed in this process " 
                  << get_num_parsed( ) << std::endl;
   			continue;
   			}
   		if ( userInput == "shutdown" ) 
            {
   			std::cout << "Are you sure? (type 'YES')\n";
   			std::cin >>  userInput;
   			if ( userInput == "YES" ) 
               {
   				std::cout << "Really really sure? (type 'JAEYOON')\n";
   				std::cin >> userInput;
   				if ( userInput == "JAEYOON" ) 
                  {
   					std::cout << "OK... Bye World" <<  std::endl;
   					break;
   					}
      			}
            }

         if ( userInput == "add" )
            {
            std::cout << "Provide a filename for urls to block\n";
            std::cin >> userInput;
            
            addBlockedHosts( userInput );
            }
         }
      while ( std::cin >> userInput );

      std::cout << "before initiate shutdown." << std::endl;
      initiate_shut_down( );
      std::cout << "after initiate shutdown." << std::endl;
      page_store_shutdown( );
      std::cout << "Shutting down." << std::endl;
      for ( auto &thread : threads ) 
         {
         thread.join( );
         std::cout << "joined" << std::endl;
         }
      } 
   catch ( const ArgError & )
      {
		std::cerr << "Usage: " << argv[0]
      		<< " [-p port] [-o hostname] [-t threads]\n\n"
      		<< "The `port' parameter accepts an integer in the range "
      		<< "[1024, 65536). Default value: `" << DefaultPort << "'\n"
      		<< "The `hostname' parameter accepts a valid filename. Default value: `"
      		<< DefaultHostname << "'\n"
      		<< "The `threads' parameter accepts a valid non-negative integer. Default value: `"
      		<< NumThreadsToSpawn << "'\n";

		return 1;
      }

fb::SizeT parseArguments( int argc, char **argv ) 
   {
	option long_opts[ ] = 
      {
		{ "hostname",  required_argument, nullptr, 'o' },
		{ "port",      required_argument, nullptr, 'p' },
		{ "help",      no_argument,       nullptr, 'h' },
		{ "threads",   no_argument,       nullptr, 't' },
      { "urls",      no_argument,       nullptr, 'u' },
		{ nullptr, 0, nullptr, 0 }
		};
	opterr = true;

	int option_idx;
	auto choice = 0;

	fb::String hostname, port, threads;

	while ( ( choice =
					getopt_long( argc, argv, "o:p:ht:u", long_opts, &option_idx ) )
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
   		case 't':
				threads = optarg;
				break;
         case 'u':
            addUrls = false;
            break;
   		case 'h':
   		default:
				throw ArgError( );
   		}
   	}

	auto rootDir = getRootDir( );
	std::cout << "Writing to " << rootDir << '\n';

	auto logfileloc = rootDir + WorkerLogFile;
	logfile.open( logfileloc.data( ) );
	if ( !logfile.is_open( ) ) 
      {
		std::cerr << "Could not open logfile `" << logfileloc
				<< "'." << std::endl;
		throw ArgError( );
   	}

   addBlockedHosts( rootDir + BlockedHostsFile );

	auto pagebinloc = rootDir + PageStoreFile;
	initializeFileName( std::move( pagebinloc ) );

	auto pagestoreloc = rootDir + PageStoreCounterFile;
	PageStoreCounter::init( pagestoreloc );

	set_master_ip( {
			hostname.empty( ) ? DefaultHostname : hostname.data( ),
			port.empty( ) ? DefaultPort : port.data( )
			} );

	return threads.empty( ) ?
			NumThreadsToSpawn :
			static_cast< fb::SizeT >( stoll( threads ) );
   }

void * parsePages( void * ) 
   {
   while ( true ) 
      {
		auto urlPair = get_url_to_parse( );

		if ( urlPair.second.empty( ) ) 
         {
			endCV.signal( );
         return nullptr;
			}

		try 
         {
         ParsedUrl urlInitial( urlPair.second );

         blockedHostsLock.lock_shared( );
         if ( blockedHosts.find( urlInitial.Host ) != blockedHosts.end( ) )
         {
            blockedHostsLock.unlock_shared( );
            continue;
         }
         blockedHostsLock.unlock_shared( );

         auto downloader = HTTPDownloader( );

			fb::String result = downloader.PrintHtml( urlPair.second );

         ParsedUrl url( downloader.finalUrl );

			Parser parser( result, std::move( url ), addUrls );
			parser.parse( );

			fb::Vector<fb::String> urls;
			for( auto iter : parser.urls )
				urls.pushBack( iter );

			add_parsed( { urlPair.first, urls } );
			addPage( std::move(parser.extractPage( urlPair.first ) ) );
			}
      catch ( ConnectionException e ) 
         {
			}
      }

	log( logfile, "True evaluating to false?\n" );
	return nullptr;
   }
