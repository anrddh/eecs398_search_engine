// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include "string.hpp"
#include "Vector.hpp"
#include "Exception.hpp"
#include <arpa/inet.h> // htonl and ntohl

// TCP messaging protocol
// Every message starts with verfication_code
// Child Machine: First letter R (char) - request urls to parse
// Master Machine: First 4 bytes: int number of urls actually give
//                   sends url_offset (uint64_t), url (string)
//                   All urls are null terminated
// Child Machine: First letter S (char),  number of pages (int)
//    [ url_offset (int), num_links (int), [ str_len (int), str, anchor_len (int), anchor_text] 
//    x num_links many times ] x NUM_URLS_PER_SEND

class SocketException : SocketException {
   SocketSocketException(const char *msg) : SocketException(msg) {}
}

// number of parsed pages info we send at a time
constexpr int NUM_PAGES_PER_RETURN = 100;

// The minimum number of pages in buffer before slave will ask
// the master for more pages to parse
constexpr int MIN_BUFFER_SIZE = 500;

// Number of retries in case of socket failure
constexpr int NUM_RETRY = 3;

// every TCP message must start with sending this value
// and will be checked for the correct code
const uint32_t VERFICATION_CODE = 1513424; 

struct ParsedPage {
   SizeT url_offset;
   fb::Vector< Pair<fb::String, fb::String> > links; // link, anchor text (concatenated)
}

void send_int(int sock, uint32_t num);
uint32_t recv_int(int sock);
void send_uint64_t(int sock, uint64_t num);
uint64_t recv_uint64_t(int sock);
void send_str(int sock, const fb::StringView str);
fb::String recv_str(int sock);
