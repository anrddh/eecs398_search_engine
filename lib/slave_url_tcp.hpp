// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include "string.hpp"

constexpr int NUM_URLS_PER_SEND = 100; // number of parsed pages we send per 

// This function must be run ONLY ONCE and must be called
// before any other functions are called
void set_master_ip( const String& master_ip_, int master_port_ );

struct ParsedPage {
   fb::String url;
   fb::String page;
   fb::Vector< Pair<fb::String, fb::String> > links; // link, anchor text (concatenated)
}

fb::String get_url_to_parse();
void add_parsed( ParsedPage pp );
