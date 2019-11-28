// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include <fb/string.hpp>
#include <fb/vector.hpp>
#include <fb/exception.hpp>
#include <fb/stddef.hpp>
#include <fb/string.hpp>
#include <arpa/inet.h> // htonl and ntohl

// TCP messaging protocol
// First message starts with verfication_code
// Child Machine: First letter R (char) - request urls to parse
// Master Machine: First 4 bytes: int number of urls actually give
//                   sends url_offset (uint64_t), url (string)
//                   All urls are null terminated
// Child Machine: First letter S (char),  number of pages (int)
//    [ url_offset (int), num_links (int), [ str_len (int), str ]
//    x num_links many times ] x NUM_URLS_PER_SEND
//
// Child Machine T First letter C (char) - ask if master wants to terminate
// Master responds T (char) - do terminate
//                 N (char) - not terminating
// Child responds F (char) - to indicate the worker is shutting down

class SocketException : public fb::Exception {
public:
   SocketException(fb::String msg) : fb::Exception(msg) {}
};

// The minimum number of pages in buffer before worker will ask
// the master for more pages to parse
constexpr int MIN_BUFFER_SIZE = 2000;

// Number of page per send to master
constexpr int PAGES_PER_SEND= 1000;

// Number of retries in case of socket failure
constexpr int NUM_RETRY = 3;

// every TCP message to master must start with sending this value
// and will be checked for the correct code
const uint32_t VERFICATION_CODE = 1513424;

struct ParsedPage {
   fb::SizeT url_offset; // offset of master
   fb::Vector< fb::String > links; // links
};

void send_char(int sock, char c);
char recv_char(int sock);
void send_int(int sock, uint32_t num);
uint32_t recv_int(int sock);
void send_uint64_t(int sock, uint64_t num);
uint64_t recv_uint64_t(int sock);
void send_str(int sock, const fb::StringView str);
fb::String recv_str(int sock);
