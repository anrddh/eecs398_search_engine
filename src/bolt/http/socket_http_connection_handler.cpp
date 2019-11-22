#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream> //remove later

#include "bolt/http/socket_http_connection_handler.hpp"
#include "bolt/constants.hpp"

SocketHttpConnection::SocketHttpConnection(std::unique_ptr<char[]> rawRequest,
                                           int len, int sock)
    : requestBuf(std::move(rawRequest)), length(len), clientHandle(sock) { }

std::unique_ptr<char[]>& SocketHttpConnection::getRawRequest() 
   {
   return requestBuf;
   }

void SocketHttpConnection::setRawResponse(std::unique_ptr<char[]> response,
                                          int len) 
   {
   responseBuf = std::make_unique<char[]>(len);
   memcpy(responseBuf.get(), response.get(), len);
   this->length = len;
   }

std::unique_ptr<char[]> SocketHttpConnection::getRawResponse() 
   {
   return std::move(responseBuf);
   }

int SocketHttpConnection::getRawRequestLength() { return length; }

int SocketHttpConnection::getRawResponseLength() { return length; }

int SocketHttpConnection::getClientHandle() { return clientHandle; }

void SocketHttpConnectionHandler::setup() {
   sockaddr_in address;
   int addrlen = sizeof(address);
   if ((serverHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
      {
      perror("cannot create socket");
      exit(0);
      }
   int yes = 1;
   setsockopt(serverHandle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
   memset((char *)&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = htonl(INADDR_ANY);
   address.sin_port = htons(PORT);
   if (bind(serverHandle, (sockaddr *)&address, (socklen_t)addrlen) < 0) 
      {
      perror("bind failed");
      exit(0);
      }
   if (listen(serverHandle, 3) < 0) 
      {
      perror("In listen");
      exit(EXIT_FAILURE);
      }
   }

std::unique_ptr<HttpConnection> SocketHttpConnectionHandler::getRequest() 
   {
   int clientHandle;
   if ((clientHandle = accept(serverHandle, 0, 0)) < 0) 
      {
      perror("In accept");
      exit(EXIT_FAILURE);
      }

   // read and respond to client request
   char requestBuffer[30000] = {0};
   int bytesRead = read(clientHandle, requestBuffer, 30000);
   if (bytesRead < 1) 
      {
      throw fb::Exception("conection failed");
      }

   /*for(int i = 0; i < bytesRead; ++i) {
      std::cout << requestBuffer[i];
   }*/
   std::unique_ptr<char[]> uniqueBuffer = std::make_unique<char[]>(bytesRead);
   memcpy(uniqueBuffer.get(), requestBuffer, bytesRead);

   std::unique_ptr<HttpConnection> conn = std::make_unique<SocketHttpConnection>(
         std::move(uniqueBuffer), bytesRead, clientHandle);

   return conn;
   }

void SocketHttpConnectionHandler::sendResponse(
    std::unique_ptr<HttpConnection> conn) {
  SocketHttpConnection *sockConn = (SocketHttpConnection *)conn.get();
  int bytesWritten =
      write(sockConn->getClientHandle(), sockConn->getRawResponse().get(),
            sockConn->getRawResponseLength());

   if(bytesWritten == 0) {
      // do something
   }

  close(sockConn->getClientHandle());
}