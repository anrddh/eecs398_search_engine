// Added by Jaeyoon Kim 11/16/2019
#pragma once

// Only one thread should be running this function
// Handles all incoming tcp messages/requests
// Takes a pointer to a valid socket (int*) for the file descriptor
// Doesn't actually return anything
// the (void *) required for passing this function to make a thread
void* handle_socket(void* port_ptr); 

// This function should be called only once
// Sends message to all children to shutdown (blocks until all 
// children receives the terminate message
void terminate_workers();
