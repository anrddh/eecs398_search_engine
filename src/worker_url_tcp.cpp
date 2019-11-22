// Added by Jaeyoon Kim 11/15/2019
#include <tcp/url_tcp.hpp>
#include <tcp/worker_url_tcp.hpp>

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

String master_ip;
int master_port;

// Constantly talks to master
void* talk_to_master(void*);

void set_master_ip( const String& master_ip_, int master_port_ ) {
   master_ip = master_ip_;
   master_port = master_port_;
   Thread t(talk_to_master, nullptr);
   t.detach();
}

Mutex to_parse_m;
CV to_parse_cv;
Queue<Pair<SizeT, String>> urls_to_parse;

Mutex parsed_m;
Vector< ParsedPage > urls_parsed; // first val url, second val parsed page

// helper function
void send_parsed_pages(int sock, Vector<ParsedPage> pages_to_send);
Vector< Pair<SizeT, String> > checkout_urls(int sock);

std::atomic<bool> shutting_down = false;

bool should_shutdown() {
   AutoLock<Mutex> l(to_parse_m);
   return shutting_down && urls_to_parse.empty();
}

void initiate_shut_down() {
   shutting_down = true;
}

// get_url_to_parse will return an
// url to parse and its unique id (offset)
// from master
Pair<SizeT, String> get_url_to_parse() {
   to_parse_m.lock();
   while ( urls_to_parse.empty() ) {
      if (shutting_down) {
         to_parse_m.unlock();
         return {0, ""};
      }

      to_parse_cv.wait(to_parse_m);
   }

   Pair<SizeT, String> url_pair = std::move(urls_to_parse.front());
   urls_to_parse.pop();
   to_parse_m.unlock();
   return url_pair;
}

void add_parsed( ParsedPage pp ) {
   parsed_m.lock();
   urls_parsed.pushBack(std::move(pp));
   parsed_m.unlock();
   return;
}

int open_socket_to_master() {
   int sock = 0, valread;
   struct sockaddr_in serv_addr;
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      throw SocketException("TCP socket: Failed to construct socket");
   }

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(master_port);

   // Convert IPv4 and IPv6 addresses from text to binary form
   if(inet_pton(AF_INET, master_ip.data(), &serv_addr.sin_addr) <= 0)
   {
      throw SocketException("TCP socket: Failed in inet_pton");
   }

   if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      throw SocketException("TCP socket: Failed in connect");
   }

   // Finished establishing socket
   // Send verfication message
   send_int(sock, VERFICATION_CODE);

   return sock;
}

void* talk_to_master_helper(int sock) {
   while (true) {
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

      // Check if we should terminate
      // and terminate accordingly
      if (shutting_down) 
      {
         return nullptr;
      }

      send_char(sock, 'T');
      char terminate_state = recv_char(sock);

      if ( terminate_state == 'T' ) 
      {
         shutting_down = true;
         return nullptr;
      } 
      else if ( terminate_state != 'N' ) 
      {
         throw SocketException("Invalid terminate state");
      }

      // If we are short on urls to parse,
      // request for more
      to_parse_m.lock();
      if (urls_to_parse.size() < MIN_BUFFER_SIZE)
      {
         to_parse_m.unlock();
         Vector< Pair<SizeT, String> > urls = checkout_urls(sock);
         to_parse_m.lock();
         for ( int i = 0; i < urls.size(); ++i )
         {
            urls_to_parse.push( std::move( urls[i] ) );
         }

         to_parse_cv.broadcast();
         to_parse_m.unlock();
      }
      else
      {
         to_parse_m.unlock();
      }
   }
}

void* talk_to_master(void*) {
   while (true) {
      FileDesc sock(open_socket_to_master());
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
      for ( int j = 0; j < page.links.size(); ++j) {
         send_str( sock, page.links[j].first );
         send_str( sock, page.links[j].second );
      }
   }

   assert(pages_to_send.empty());
}
