// Created by Jaeyoon Kim 11/13/2019
#pragma once
#include "fb/string.hpp"
#include "fb/vector.hpp"
#include "url_tcp.hpp"
#include "disk/frontier.hpp"

// Given offsets for urls, sends over offset adn urls (by reading the saved urls)
void send_urls(int sock, const FrontierUrl* urls_to_parse);

// Given offsets for urls, sends over offset adn urls (by reading the saved urls)
void send_urls(int sock, const fb::Vector<fb::SizeT>& urls_to_parse);

fb::Vector<ParsedPage> recv_parsed_pages(int sock);
