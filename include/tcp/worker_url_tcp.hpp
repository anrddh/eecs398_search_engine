// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include <fb/string.hpp>
#include <fb/string_view.hpp>
#include <tcp/url_tcp.hpp>
#include <tcp/addr_info.hpp>

// This is a TCP inteface for worker computers


// This function must be run ONLY ONCE and must be called
// before any other functions are called
void set_master_ip(AddrInfo loc);

// Tell system to initiate shut down
// We will no longer recieve any new pages to parse from master
// However, all urls we already have will be parsed first.
// This can be remotely called by master
void initiate_shut_down();

// Checks if we should shut down
// (i.e. no more urls to parse)
bool should_shutdown();

void print_tcp_status();

// If there are no more urls to parse
// the empty url will be returned
// The thread that got an empty url should stop parsing
// since there are no more urls that needs to be parsed
fb::Pair<fb::SizeT, fb::String> get_url_to_parse();
void add_parsed( ParsedPage&& pp ); // Use move ctor if possible
