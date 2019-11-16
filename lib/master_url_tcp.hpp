// Created by Jaeyoon Kim 11/13/2019
#pragma once
#include "string.hpp"
#include "vector.hpp"
#include "url_tcp.hpp"

// Checks if socket is valid, and 
// returns the message type
// 'S': the slave is sending parsed page info
// 'R': the slave is requesting more urls to parse
// Might throw SocketException
char check_socket(int sock);

// Given offsets for urls, sends over offset adn urls (by reading the saved urls)
void send_urls(int sock, const fb::Vector<fb::SizeT>& urls_to_parse);

fb::Vector<ParsedPage> recv_parsed_pages(int sock);
