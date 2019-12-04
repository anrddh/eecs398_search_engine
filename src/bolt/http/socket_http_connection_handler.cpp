#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream> //remove later

#include "bolt/http/socket_http_connection_handler.hpp"
#include "bolt/constants.hpp"

SocketHttpConnection::SocketHttpConnection(fb::UniquePtr<char[]> rawRequest,
                                           int len, int sock)
    : requestBuf(std::move(rawRequest)), length(len), clientHandle(sock) { }

fb::UniquePtr<char[]>& SocketHttpConnection::getRawRequest() 
   {
   return requestBuf;
   }

void SocketHttpConnection::setRawResponse(fb::UniquePtr<char[]> response,
                                          int len) 
   {
   responseBuf = fb::makeUnique<char[]>(len);
   memcpy(responseBuf.get(), response.get(), len);
   this->length = len;
   }

fb::UniquePtr<char[]> SocketHttpConnection::getRawResponse() 
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

fb::UniquePtr<HttpConnection> SocketHttpConnectionHandler::getRequest() 
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

   fb::UniquePtr<char[]> uniqueBuffer = fb::makeUnique<char[]>(bytesRead);
   memcpy(uniqueBuffer.get(), requestBuffer, bytesRead);

   fb::UniquePtr<HttpConnection> conn = fb::makeUnique<SocketHttpConnection>(
         std::move(uniqueBuffer), bytesRead, clientHandle);

   return conn;
   }

void SocketHttpConnectionHandler::sendResponse(
   fb::UniquePtr<HttpConnection> conn) {
   SocketHttpConnection *sockConn = (SocketHttpConnection *)conn.get();
   std::cout << std::endl << std::endl << "Response: " << std::endl;
   fb::UniquePtr<char[]> response = sockConn->getRawResponse( );
   for(int i = 0; i < sockConn->getRawResponseLength( ); ++i )
      {
      std::cout << response[i];
      }

   std::cout << std::endl << std::endl;
   int bytesWritten =
      write(sockConn->getClientHandle(), response.get(),
            sockConn->getRawResponseLength());

   if(bytesWritten == 0) {
      // do something
   }

   close(sockConn->getClientHandle());
}
