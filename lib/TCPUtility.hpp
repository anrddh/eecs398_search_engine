// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include "string.hpp"

void send_str(int sock, const fb::StringView str) {
   int32_t size = str.size();
   send(sock , &size , sizeof(size) , 0 );

   // Should send null character as well
   if (send(sock , &str.data() , size + 1 , 0 ) == -1) {
      // TODO throw an exception
   }
}

fb::String recv_str(int sock) {
   int32_t size;
   if (recv(sock, &size, sizeof(int32_t), MSG_WAITALL) == -1) {
      // TODO throw an exception
   }

   String url;
   url.resize( size ); // resize to length of string (not counting null character)
   
   // Need to write null character as well
   if (recv(sock, url.data, size + 1, MSG_WAITALL) == -1) {
      // TODO throw an exception
   }

   return url;
}
