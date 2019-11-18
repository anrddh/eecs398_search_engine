// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include "../../lib/string.hpp"
#include "url_tcp.hpp"

// This is a TCP inteface for worker computers


// This function must be run ONLY ONCE and must be called
// before any other functions are called
void set_master_ip( const fb::String& master_ip_, int master_port_ );

// Tell system to initiate shut down
// We will no longer recieve any new pages to parse from master
// However, all urls we already have will be parsed first.
void initiate_shut_down();

// Checks if we should shut down
// (i.e. no more urls to parse)
bool should_shutdown();


// If 
fb::Pair<fb::SizeT, fb::String> get_url_to_parse();
void add_parsed( ParsedPage pp ); // Use move ctor if possible
