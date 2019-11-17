// Added by Jaeyoon Kim 11/16/2019
#include "handle_socket.hpp"
#include "master_url_tcp.hpp"
#include "frontier_pool.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/file_descriptor.hpp"
#include <sys/socket.h> 

using namespace fb;

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket 
// and the socket will be closed
void* handle_request(void* sock_ptr);

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket 
// and the socket will be closed
void* handle_send(void* sock_ptr);

void* handle_socket(void* sock_ptr) {
   int server_fd = * (FileDesc * ) sock_ptr;
   int sock;
   delete (FileDesc *) sock_ptr;

   while (true) {
      if (listen(server_fd, 3) < 0) 
      { 
          perror("listen"); 
          exit(EXIT_FAILURE); 
      } 

      if ((sock = accept(server_fd, nullptr, nullptr)<0))
      { 
          perror("accept"); 
          exit(EXIT_FAILURE); 
      } 

      try {
         char message_type = check_socket( sock );
         // worker process is sending parsed pages
         if ( message_type == 'S' ) {
            Thread t(handle_send, new int( sock ));
            t.detach();

         // worker process is requesting more pages to parse
         } else if ( message_type = 'R')  {
            Thread t(handle_request, new int( sock ));
            t.detach();
         } else {
            throw SocketException("Invalid message type");
         }
      } catch (SocketException& se) {
         // TODO log this 
         return nullptr;
      }
   }
}

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket 
// and the socket will be closed
void* handle_send(void* sock_ptr) {
   FileDesc sock(* (int *) sock_ptr);
   delete (int *) sock_ptr;

   try {
      Vector<ParsedPage> pages = recv_parsed_pages(sock);
      // TODO insert them into containers
   } catch (SocketException& se) {
      // TODO log error
      return nullptr;
   }
   return nullptr;
}

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket 
// and the socket will be closed
void* handle_request(void* sock_ptr) {
   FileDesc sock(* (int *) sock_ptr);
   delete (int *) sock_ptr;
   //Vector<SizeT> urls_to_parse = frontierGetUrls(); // TODO add it back it
   Vector<SizeT> urls_to_parse = {1, 2, 3};
   try {
      send_urls(sock, urls_to_parse);
   } catch (SocketException& se) {
      // TODO log this and add back urls to the frontier
   }

   return nullptr;
}
