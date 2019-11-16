// Added by Jaeyoon Kim 11/16/2019
#include "handle_socket.hpp"

using namespace fb;

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket 
// and the socket will be closed
void* handle_request(void* sock_ptr);

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket 
// and the socket will be closed
void* handle_send(void* sock_ptr);

void* handle_socket(void* port_ptr) {
   int port = * (int *) port_ptr;
   delete (int *) port_ptr;
   int server_fd, sock, valread; 
   struct sockaddr_in address; 
   int opt = 1; 
   int addrlen = sizeof(address); 
      
   // Creating socket file descriptor 
   if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
   { 
       perror("socket failed"); 
       exit(EXIT_FAILURE); 
   } 
      
   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                 &opt, sizeof(opt))) 
   { 
       perror("setsockopt"); 
       exit(EXIT_FAILURE); 
   } 
   address.sin_family = AF_INET; 
   address.sin_addr.s_addr = INADDR_ANY; 
   address.sin_port = htons( port ); 
      
   // Forcefully attaching socket to the port 8080 
   if (bind(server_fd, (struct sockaddr *)&address,  
                                sizeof(address))<0) 
   { 
       perror("bind failed"); 
       exit(EXIT_FAILURE); 
   } 

   while (true) {
      if (listen(server_fd, 3) < 0) 
      { 
          perror("listen"); 
          exit(EXIT_FAILURE); 
      } 

      if ((sock = accept(server_fd, (struct sockaddr *)&address,  
                         (socklen_t*)&addrlen))<0) 
      { 
          perror("accept"); 
          exit(EXIT_FAILURE); 
      } 

      try {
         char message_type = check_socket( sock );
         // worker process is sending parsed pages
         if ( message_type == 'S' ) {

         // worker process is requesting more pages to parse
         } else if ( message_type = 'R')  {
         } else {
            throw SocketException("Invalid message type");
         }
      } catch (SoceketException& se) {
         // TODO log this 
         return;
      }


      }

      Thread t(handle_socket_helper, new int( sock ));
      t.detach();
   }
}

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket 
// and the socket will be closed
void* handle_send(void* sock_ptr) {
   FileDesc sock(* (int *) sock_ptr);
   delete sock_ptr;

   try {
      Vector<ParsedPage> pages = recv_urls(sock);
      // TODO insert them into containers
   } catch (SoceketException& se) {
      // TODO log error
      return;
   } catch (...) {
      return;
   }
}

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket 
// and the socket will be closed
void* handle_request(void* sock_ptr) {
   FileDesc sock(* (int *) sock_ptr);
   delete sock_ptr;
   Vector<SizeT> urls_to_parse = frontierGetUrls();
   Vector< Pair<SizeT, String >> url_pair; // TODO convert urls_to_parse to offset and url string
   try {
      send_urls(sock, url_pair);
   } catch (SoceketException& se) {
      // TODO log this and add back urls to the frontier
      return;
   } catch (...) {
      return;
   }
}
