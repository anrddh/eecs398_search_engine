// Added by Jaeyoon Kim 11/15/2019

#include "master_url_tcp.hpp"
#include "handle_socket.hpp"
#include "../../lib/string.hpp"
#include "../../lib/vector.hpp"
#include "../../lib/thread.hpp"
#include "../../lib/file_descriptor.hpp"
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

    cout << "got server socket of " << server_fd << endl;

      std::cout << "before listen" << std::endl;
      if (listen(server_fd, 3) < 0) 
      { 
         std::cout << "listen error" << std::endl;
         std::cout << errno << std::endl;
          perror("listen"); 
          exit(EXIT_FAILURE); 
      } 
      std::cout << "after listen" << std::endl;

    /*
    Thread t(handle_socket, new FileDesc(server_fd));
    t.detach();
    */

    while (true);

    return 0; 
} 
