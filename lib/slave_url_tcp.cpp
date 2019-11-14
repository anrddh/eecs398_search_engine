#include "transfer_url.hpp"
#include "TCPUtility.hpp"
#include "vector.hpp"
#include "queue.hpp"
#include "mutex.hpp"
#include "utility.hpp"

using namespace fb;

// TCP messaging protocol
// Child Machine: First letter R (char) - request NUM_URLS_PER_REQUEST urls to parse
// Master Machine: First 4 bytes: int number of urls actually give
//                   All urls are null terminated
// Child Machine: First letter S (char), 
//    [ url_size (int), url, file_size (int) file, num_links (int), 
//    [ str_len (int), str, 
//    anchor_len (int), anchor_text] x num_links many times ] x NUM_URLS_PER_SEND

String master_ip;
int master_port;

void set_master_ip( const String& master_ip_, int master_port_ ) {
   master_ip = master_ip_;
   master_port = master_port_;
}

Mutex to_parse_m;
Queue<String> urls_to_parse;
bool getting_more = false; // Check if some thread is getting more urls

Mutex parsed_m;
Vector< ParsedPage > urls_parsed; // first val url, second val parsed page

String get_url_to_parse() {
   to_parse_m.lock();
   if ( urls_to_parse.size() << 
   while ( urls_to_parse

   to_parse_m.unlock();
}

void add_parsed( ParsedPage pp ) {
   parsed_m.lock();
   urls_parsed.push_back(std::move(pp));

   if ( urls_parsed.size() != NUM_URLS_PER_SEND ) {
      parsed_m.unlock();
      return;
   }

   // Swap out the urls_parse so that we can unlock quicker
   Queue<String> swapped_to_parse;
   swap(swapped_to_parse, urls_to_parse);
   parsed_m.unlock();

   send_parsed_pages( swapped_to_parse );
}

void checkout_urls() {
   int sock = 0, valread;
   struct sockaddr_in serv_addr;
   char *hello = "Hello from client";
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      // TODO throw an exception
   }

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(master_port);

   // Convert IPv4 and IPv6 addresses from text to binary form
   if(inet_pton(AF_INET, master_ip.c_str(), &serv_addr.sin_addr) <= 0)
   {
      // TODO throw an exception
   }

   if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      // TODO throw an exception
   }

   char request_code = 'R';
   if (send(sock , &request_code , sizeof(request_code) , 0 ) == -1) {
      // TODO throw an exception
   }

   int32_t num_urls;
   if (recv(sock, &num_urls, sizeof(int32_t), MSG_WAITALL) == -1) {
      // TODO throw an exception
   }

   Vector<String> received_urls;
   for (int i = 0; i < num_urls; ++i) {
      received_urls.push_back( recv_url() );
   }

   to_parse_m.lock();
   for ( int i = 0; i < urls_to_parse.size(); ++i ) {
      urls_to_parse.push_back( std::move( urls_to_parse[i] ) );
   }
   to_parse_m.unlock();
}

// Child Machine: First letter S (char), 
//    [ url_size (int), url, file_size (int) file, num_links (int), 
//    [ str_len (int), str, 
//    anchor_len (int), anchor_text] x num_links many times ] x NUM_URLS_PER_SEND
void send_parsed_pages(Vector<ParsedPage> pages_to_send) {
   int sock = 0, valread;
   struct sockaddr_in serv_addr;
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      // TODO throw an exception
   }

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(master_port);

   // Convert IPv4 and IPv6 addresses from text to binary form
   if(inet_pton(AF_INET, master_ip.c_str(), &serv_addr.sin_addr) <= 0)
   {
      // TODO throw an exception
   }

   if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      // TODO throw an exception
   }

   char request_code = 'S';
   send( sock , &request_code , sizeof(request), 0 );

   if (send(sock , &request_code , size + 1 , 0 ) == -1) {
      // TODO throw an exception
   }

   while ( !pages_to_send.empty() ) {
      send_str( pages_to_send.back().url );
      send_str( pages_to_send.back().page );
      int num_links = pages_to_send.back().links.size();
      send( sock , &num_links, sizeof( num_links ), 0 );

      for ( int i = 0; i < num_links; ++i ) {
         send_str( pages_to_send.back().links[i].first );
         send_str( pages_to_send.back().links[i].second );
      }
   }
}
