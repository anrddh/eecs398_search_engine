#include <parse/parser.hpp>
#include <http/download_html.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <disk/page_store.hpp>

#include <fb/thread.hpp>
#include <fb/cv.hpp>
#include <fb/mutex.hpp>

#include <iostream>

constexpr int NUM_THREAD = 50;

fb::Mutex endLock;
fb::CV endCV;

void *parsePages( void * )
   {
   while( true )
      {
      auto urlPair = get_url_to_parse( );

	  if ( urlPair.second.empty( ) )
         {
         std::cout << "Got empty link" << std::endl;
         endCV.signal();
         return nullptr;
         }

	  try
         {
         auto downloader = HTTPDownloader( );
         fb::String result = downloader.PrintHtml( urlPair.second );

         ParsedUrl url( downloader.finalUrl );

         fb::Parser parser( result, url );

         parser.parse( );

         fb::Vector< fb::Pair<fb::String, fb::String> > links;

         add_parsed( { urlPair.first, parser.urlAnchorText.covert_to_vec() } );

         addPage( parser.extractPage( urlPair.first ) ); // TODO I think move ctor will be called? -Jaeyoon
   		}
		catch ( ConnectionException e )
   		{
         }
      }
   std::cout << "True evaluating to false?" << std::endl;
   return nullptr;
   }

void *commandLineArgs( void * )
   {
   fb::String userInput;
   do
	  {
	  std::cout << "Shut down? (type 'YES')" << std::endl;
	  std::cin >> userInput;
	  if ( userInput == "YES" )
		 {
		 std::cout << "Are you sure? (type 'YES')" << std::endl;
		 std::cin >>  userInput;
		 if ( userInput == "YES" )
			{
			std::cout << "Really really sure? (type 'YES')" << std::endl;
			std::cin >> userInput;
			if ( userInput == "YES" )
			   {
			   std::cout << "OK... Bye World" <<  std::endl;
			   break;
			   }
			}
		 }
	  }
   while ( std::cin >> userInput );
   initiate_shut_down( );
   return nullptr;
   }

int main( int, char **argv )
   {
   std::cout << argv[1] << std::endl;
   std::cout << atoi(argv[2]) << std::endl;
   set_master_ip( argv[1], atoi(argv[2]) );
   fb::Thread argsThreads( commandLineArgs, nullptr );

   SSLWrapper::SSLInit( );
   initializeFileName( "/tmp/page_store" );

   fb::Vector<fb::Thread> threads;
   for ( int i = 0;  i < NUM_THREAD;  ++i )
	  threads.emplaceBack( parsePages, nullptr );

   endLock.lock( );

   while ( !should_shutdown( ) )
	  endCV.wait( endLock );

   std::cout << "Shutting down" << std::endl;
   endLock.unlock( );

   for ( int i = 0;  i < NUM_THREAD;  ++i )
	  threads[i].join( );

   argsThreads.join( );

   }
