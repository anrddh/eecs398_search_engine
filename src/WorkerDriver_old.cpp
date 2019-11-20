#include "parser.hpp"
#include "DownloadHTML.hpp"
#include "worker_url_tcp.hpp"

#include "../../lib/thread.hpp"
#include "../../lib/cv.hpp"
#include "../../lib/mutex.hpp"

#include <iostream>

constexpr int NUM_THREAD = 100;

fb::Mutex endLock;
fb::CV endCV;

void *parsePages( void * ) 
   {
   while( true )
      {
   	auto urlPair = get_url_to_parse( );

      if ( urlPair.second.empty( ) )
         {
         endCV.signal();
         return nullptr;
         }


      auto downloader = HTTPDownloader( );
      fb::String result = downloader.PrintHtml( urlPair.second );

      ParsedUrl url( downloader.finalUrl );

      auto parser = fb::Parser( result,
            url.Service + "://" + url.Host );

      fb::Vector< fb::Pair<fb::String, fb::String> > links;

      for ( auto iter = parser.urlAnchorText.begin( );  
            iter != parser.urlAnchorText.end( );  ++iter )
         links.emplaceBack( iter.key( ), *iter );

      ParsedPage pp = { urlPair.first, links };

      add_parsed( pp );
      }
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
   }

int main( int argc, char **argv )
   {
   fb::Thread argsThreads( commandLineArgs, nullptr );

   fb::Vector<fb::Thread> threads;
   for ( int i = 0;  i < NUM_THREAD;  ++i )
      threads.emplaceBack( parsePages, nullptr );

   endLock.lock( );

   while ( !should_shutdown( ) )
      endCV.wait( endLock );

   endLock.unlock( );

   for ( int i = 0;  i < NUM_THREAD;  ++i )
      threads[i].join( );

   argsThreads.join( );

   }