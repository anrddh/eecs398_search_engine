// Added by Jaeyoon Kim 11/15/2019
#include <tcp/url_tcp.hpp>
#include <tcp/worker_url_tcp.hpp>
#include <tcp/addr_info.hpp>

#include <fb/vector.hpp>
#include <fb/queue.hpp>
#include <fb/mutex.hpp>
#include <fb/cv.hpp>
#include <fb/utility.hpp>
#include <fb/file_descriptor.hpp>
#include <fb/exception.hpp>
#include <fb/thread.hpp>

#include <cassert>
#include <string.h>
#include <errno.h>
#include <atomic>

using namespace fb;

AddrInfo masterLoc;

// Constantly talks to master
void* talk_to_master(void*);

void set_master_ip(AddrInfo loc) {
   masterLoc = std::move(loc);
   Thread t(talk_to_master, nullptr);
   t.detach();
}

Mutex to_parse_m;
CV to_parse_cv;
Queue<Pair<SizeT, String>> urls_to_parse;


Mutex parsed_m;
Vector< ParsedPage > urls_parsed; // first val url, second val parsed page

void print_tcp_status() {
   AutoLock<Mutex> l1(parsed_m);
   AutoLock<Mutex> l2(to_parse_m);
   std::cout << "Num urls to parse " << urls_to_parse.size() << " "
             << " num urls parsed " << urls_parsed.size() << std::endl;
}

// helper function
void send_parsed_pages(int sock, Vector<ParsedPage> pages_to_send);
Vector< Pair<SizeT, String> > checkout_urls(int sock);

std::atomic<bool> shutting_down = false;

void initiate_shut_down() {
   shutting_down = true;
   to_parse_cv.broadcast();
}

// get_url_to_parse will return an
// url to parse and its unique id (offset)
// from master
Pair<SizeT, String> get_url_to_parse() {
   AutoLock l(to_parse_m);
   while ( urls_to_parse.empty() ) {
      if (shutting_down)
         break;

      to_parse_cv.wait(to_parse_m);
   }

   if (shutting_down)
      return {0, ""};

   Pair<SizeT, String> url_pair = std::move(urls_to_parse.front());
   urls_to_parse.pop();
   return url_pair;
}

void add_parsed( ParsedPage&& pp ) {
   AutoLock<Mutex> l(parsed_m);
   urls_parsed.pushBack( std::move(pp) );
}

fb::FileDesc open_socket_to_master() {
   auto sock = masterLoc.getConnectedSocket();

   // Finished establishing socket
   // Send verfication message
   send_int(sock, VERFICATION_CODE);

   return sock;
}

void* talk_to_master_helper(int sock) {
   while (true) {
      // Check if we should terminate
      // and terminate accordingly
      if (shutting_down)
      {
         return nullptr;
      }

      // Send the parsed info
      parsed_m.lock();
      if (urls_parsed.empty())
      {
         parsed_m.unlock();
      }
      else
      {
         Vector< ParsedPage > local; // first val url, second val parsed page
         local.swap(urls_parsed);
         parsed_m.unlock();
         send_parsed_pages( sock, local );
      }

      // If we are short on urls to parse,
      // request for more
      to_parse_m.lock();
      while (urls_to_parse.size() < MIN_BUFFER_SIZE)
      {
         to_parse_m.unlock();
         Vector< Pair<SizeT, String> > urls = checkout_urls(sock);
         to_parse_m.lock();
         for ( fb::SizeT i = 0; i < urls.size(); ++i )
         {
            urls_to_parse.push( std::move( urls[i] ) );
         }

         to_parse_cv.broadcast();
      }
      to_parse_m.unlock();

      // This thread doesn't need to be running at full speed.
      // It only needs to be occasionally to check if we need to send
      // master any parsed pages or recv some extra pages and if needed
      // do some work.
      // We shouldn't constantly be checking if there is work to do
      //
      // pthread_yield is guarenteed to work on a linux system
      // but for other systems, this m
      //pthread_yield();
   }
}

void* talk_to_master(void*) {
   while (true) {
      auto sock = open_socket_to_master();
      try {
         return talk_to_master_helper(sock);
      }
      catch (SocketException& se)
      {
         std::cerr << "SocketException in talk to master. Error: " << se.what() << std::endl;
      }
   }
}

Vector< Pair<SizeT, String> > checkout_urls(int sock) {
   send_char(sock, 'R');

   int32_t num_urls = recv_int(sock);

   Vector< Pair<SizeT, String> > received_urls;

   for (int i = 0; i < num_urls; ++i) {
      SizeT url_offset = recv_uint64_t( sock );
      String url = recv_str( sock );

      received_urls.pushBack(make_pair(url_offset, std::move(url)));
   }

   return received_urls;
}

// Child Machine: First letter S (char),  number of pages (int)
//    [ url_offset (int), num_links (int), [ str_len (int), str, anchor_len (int), anchor_text]
//    x num_links many times ] x NUM_URLS_PER_SEND
void send_parsed_pages(int sock, Vector<ParsedPage> pages_to_send) {
   send_char(sock, 'S');

   int size = pages_to_send.size();
   send_int(sock, size);

   for (int i = 0; i < size; ++i) {
      assert( !pages_to_send.empty() );
      ParsedPage page = std::move( pages_to_send.back() );
      pages_to_send.popBack();
      send_uint64_t( sock, page.url_offset ); // convert back to
      send_int( sock, page.links.size() );
      for ( fb::SizeT j = 0; j < page.links.size(); ++j) {
         send_str( sock, page.links[j].first );
         send_str( sock, page.links[j].second );
      }
   }

   assert(pages_to_send.empty());
}
