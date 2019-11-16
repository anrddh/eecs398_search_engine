#include "slave_url_tcp.hpp"
#include "url_tcp.hpp"
#include "vector.hpp"
#include "queue.hpp"
#include "mutex.hpp"
#include "cv.hpp"
#include "utility.hpp"
#include "file_descriptor.hpp"
#include "Exception.hpp"
#include <cassert>
#include <string.h>
#include <errno.h>

using namespace fb;

String master_ip;
int master_port;

void set_master_ip( const String& master_ip_, int master_port_ ) {
   master_ip = master_ip_;
   master_port = master_port_;
}

Mutex to_parse_m;
CV to_parse_cv;
Queue<Pair<SizeT, String>> urls_to_parse;
bool getting_more = false; // Check if some thread is getting more urls

Mutex parsed_m;
Vector< ParsedPage > urls_parsed; // first val url, second val parsed page

// helper function
void send_parsed_pages(Vector<ParsedPage> pages_to_send);
Vector< Pair<SizeT, String> > checkout_urls();

// get_url_to_parse will return an 
// url to parse and its unique id (offset)
// from master
Pair<SizeT, String> get_url_to_parse() {
   to_parse_m.lock();
   while (true) {
      if ( urls_to_parse.size() < MIN_BUFFER_SIZE && !getting_more ) {
         Vector<Pair< SizeT, String >> new_urls;
         getting_more = true;
         to_parse_m.unlock();
         try {
            // Release the lock while processing TCP
            new_urls = std::move(checkout_urls());// apparently doesn't automatically
         } catch (const SocketException& se) {
            new_urls.clear();
            // TODO print error message
         }

         to_parse_m.lock();
         for ( int i = 0; i < new_urls.size(); ++i ) {
            urls_to_parse.push( std::move( new_urls[i] ) );
         }
         getting_more = false;
         to_parse_cv.broadcast();
      }

      if ( !urls_to_parse.empty() ) {
         Pair<SizeT, String> url_pair = std::move(urls_to_parse.front());
         to_parse_m.unlock();
         return url_pair;
      } else {
         to_parse_cv.wait(to_parse_m);
      }
   }
}

void add_parsed( ParsedPage pp ) {
   parsed_m.lock();
   urls_parsed.pushBack(std::move(pp));

   if ( urls_parsed.size() != NUM_PAGES_PER_RETURN ) {
      parsed_m.unlock();
      return;
   }

   // Swap out the urls_parse so that we can unlock quicker
   Vector<ParsedPage> swapped_to_parse;
   swap(swapped_to_parse, urls_parsed);
   parsed_m.unlock();

   send_parsed_pages( std::move( swapped_to_parse ) );
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

void send_message_type(int sock, char message_type) {
   if ( send(sock , &message_type, sizeof(message_type) , 0 ) == -1) {
      throw SocketException("TCP socket: Failed in send request code");
   }
}

Vector< Pair<SizeT, String> > checkout_urls() {
   // RAII file descriptor does automatic close() 
   // when it goes out of scope
   FileDesc sock(open_socket_to_master());

   send_message_type(sock, 'R');

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
void send_parsed_pages(Vector<ParsedPage> pages_to_send) {
   // RAII file descriptor does automatic close() 
   // when it goes out of scope
   FileDesc sock(open_socket_to_master());

   send_message_type(sock, 'S');

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
