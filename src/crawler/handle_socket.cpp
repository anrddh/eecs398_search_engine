// Added by Jaeyoon Kim 11/16/2019
#include "handle_socket.hpp"
#include "master_url_tcp.hpp"
#include "frontier_pool.hpp"
#include "UrlStore.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/file_descriptor.hpp"
#include <sys/socket.h> 

using namespace fb;

atomic<bool> do_terminate = false;

void terminate_workers() {
   do_terminate = true;
}

void* handle_socket_helper(void* sock_ptr);

void handle_request(int sock);
void handle_send(int sock);

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
      
      if (do_terminate) {
         return nullptr;
      }
      // TODO do non-blocking
      int flags = fcntl(listen_socket_fd, F_GETFL)
        guard(fcntl(listen_socket_fd, F_SETFL, flags | O_NONBLOCK), 

      if ((sock = accept(server_fd, nullptr, nullptr)<0))
      { 
          perror("accept"); 
          exit(EXIT_FAILURE); 
      } 

      Thread t(handle_request_helper, new int( sock ));
      t.detach();
   }
}

void* handle_socket_helper(void* sock_ptr) {
   FileDesc sock(* (int *) sock_ptr);
   delete (int *) sock_ptr;

   try {
      while (true)  {
         char message_type = recv_char(sock);
         if (message_type == 'R') {
            handle_request(sock);
         } else if (message_type == 'S') {
            handle_send(sock);
         } else if (message_type == 'T')  {

         } else {
            throw SocketException("Wrong message type");
         }
      }
   } catch (SocketException& se) {
      // TODO log error
      return nullptr;
   }
}

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket 
// and the socket will be closed
void handle_send(int sock) {
   Vector<ParsedPage> pages = recv_parsed_pages(sock);
}

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket 
// and the socket will be closed
void* handle_request(int sock) {
   Vector<SizeT> urls_to_parse = frontierGetUrls();
   send_urls(sock, urls_to_parse);
}
