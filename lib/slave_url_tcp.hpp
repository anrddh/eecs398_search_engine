// Created by Jaeyoon Kim 11/9/2019
#pragma once
#include "string.hpp"

// This is a TCP inteface for slave computers


// This function must be run ONLY ONCE and must be called
// before any other functions are called
void set_master_ip( const String& master_ip_, int master_port_ );


fb::Pair<fb::SizeT, fb::String> get_url_to_parse();
void add_parsed( ParsedPage pp ); // Use move ctor if possible
