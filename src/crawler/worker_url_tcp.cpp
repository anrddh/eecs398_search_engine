// Added by Jaeyoon Kim 11/15/2019
#include "worker_url_tcp.hpp"
#include "url_tcp.hpp"
#include "../../lib/vector.hpp"
#include "../../lib/queue.hpp"
#include "../../lib/mutex.hpp"
#include "../../lib/cv.hpp"
#include "../../lib/utility.hpp"
#include "../../lib/file_descriptor.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/thread.hpp"
#include <cassert>
#include <string.h>
#include <errno.h>

using namespace fb;

String master_ip;
int master_port;

// Constantly talks to master
void* talk_to_master(void*);

void initialize_tcp( const String& master_ip_, int master_port_ ) {
   master_ip = master_ip_;
   master_port = master_port_;
   // TODO spawn thread 
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
   AutoLock<Mutex> al(to_parse_m);
   while ( !urls_to_parse.empty() ) {
      to_parse_cv.wait(to_parse_m);
   }

   Pair<SizeT, String> url_pair = std::move(urls_to_parse.front());
   urls_to_parse.pop();
   return url_pair();
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

void send_message_type(int sock, char message_type) {
   if ( send(sock , &message_type, sizeof(message_type) , 0 ) == -1) {
      throw SocketException("TCP socket: Failed in send request code");
   }
}

void* talk_to_master(void*) {
   while (true) {
      FileDesc sock(open_socket_to_master());
      try {
         send_char(sock, 'T');
         char terminate_state = recv_char(sock);

         if ( terminate_state == 'T' ) {
            // TODO set terminate
            return nullptr;
         } else if ( terminate_state != 'N' ) {
            throw SocketException("Invalid terminate state");
         }

         parsed_m.lock();
         if (urls_parsed.empty()) {
            parsed_m.lock();
         } else {
            Vector< ParsedPage > local; // first val url, second val parsed page
            local.swap(urls_parsed);
            parsed_m.unlock();
            send_parsed_pages( pages_to_send );
            char ack = recv_char(sock);
         }

         to_parse_m.lock();
         if (to_parse.size() < MIN_BUFFER_SIZE) 
         {
            to_parse_m.unlock();
            Vector< Pair<SizeT, String> > urls = checkout_urls();
            to_parse_m.lock();
            for ( int i = 0; i < urls.size(); ++i ) 
            {
               urls_to_parse.push( std::move( urls[i] ) );
            }

            to_parse_cv.broad_cast();
            to_parse_m.unlock();
         } 
         else 
         {
            to_parse.unlock();
         }
      } catch (SocketException& se) {
         // TODO log error
      }
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
