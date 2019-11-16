#pragma once
// Added by Jaeyoon Kim 11/16/2019


// Handles all incoming tcp messages/requests
// Takes a dynamically allocated integer pointer for the port number
// the port number is deleted by handle_socket function
// Doesn't actually return anything
// the (void *) required for passing this function to make a thread
void* handle_socket(void* port_ptr); 
