#include "master_url_tcp.hpp"

#include "string.hpp"
#include "vector.hpp"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>

using namespace std;
using namespace fb;
#define PORT 8992
int main(int argc, char const *argv[]) 
{ 
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
       
    // Forcefully attaching socket to the port 8000 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    while (true) {
       cout << "before listen" << endl;
       if (listen(server_fd, 3) < 0) 
       { 
           perror("listen"); 
           exit(EXIT_FAILURE); 
       } 
       cout << "after listen" << endl;

       if ((sock = accept(server_fd, (struct sockaddr *)&address,  
                          (socklen_t*)&addrlen))<0) 
       { 
           perror("accept"); 
           exit(EXIT_FAILURE); 
       } 
       cout << "after accept" << endl;
       try {
          char message_type = check_socket( sock );
          if ( message_type == 'S' ) {
             Vector<ParsedPage> pages = recv_parsed_pages(sock);
             for (auto p : pages) {
                cout << "offset: " << p.url_offset << "\n";
                for ( auto link : p.links ) {
                   cout << "url: " << link.first << " anchor text: " << link.second << "\n";
                }
             }
          } 
          if ( message_type == 'R' ) { 
             cout << "in request" << endl;
             Vector<SizeT> dummy_offset;
             for (int i = 0; i < 100; ++i) {
                dummy_offset.pushBack(i);
             }
             send_urls(sock, dummy_offset);
          }
       } catch (SocketException& se) {
          cout << se.what() << endl;
       } 
       close(sock);
    }

    return 0; 
} 
