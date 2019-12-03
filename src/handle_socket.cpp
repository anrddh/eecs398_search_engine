// Added by Jaeyoon Kim 11/16/2019
#include "tcp/handle_socket.hpp"
#include "tcp/master_url_tcp.hpp"
#include "disk/frontier.hpp"
#include "disk/url_store.hpp"
#include "disk/UrlInfo.hpp"
#include "fb/exception.hpp"
#include "fb/thread.hpp"
#include "fb/cv.hpp"
#include "fb/file_descriptor.hpp"
#include <sys/socket.h>
#include <iostream>
#include <errno.h>

using namespace fb;


Mutex term_mtx;
CV term_cv;
int num_threads = 0;
bool do_terminate = false;

class ThreadLifeTracker {
public:
   ThreadLifeTracker()
   {
      fb::AutoLock l(term_mtx);
      ++num_threads;
   }

   ~ThreadLifeTracker()
   {
      fb::AutoLock l(term_mtx);
      if (--num_threads == 0)
         term_cv.signal();
   }
};

void terminate_workers() {
   fb::AutoLock l(term_mtx);
   do_terminate = true;
   while (num_threads != 0)
      term_cv.wait(term_mtx);
}

void* handle_socket_helper(void* sock_ptr);

void handle_request(int sock);
void handle_send(int sock);

void* handle_socket(void* sock_ptr) {
   int server_fd = * (int *) sock_ptr;
   int sock;

   if (listen(server_fd, 3) < 0) {
       perror("listen");
       exit(EXIT_FAILURE);
   }

   while (true) {
      term_mtx.lock();
      if (do_terminate) {
         term_mtx.unlock();
         return nullptr;
      }
      term_mtx.unlock();

      if ((sock = accept(server_fd, nullptr, nullptr)) < 0)
      {
          perror("accept");
          exit(EXIT_FAILURE);
      }

      term_mtx.lock();
      if (do_terminate) {
         term_mtx.unlock();
         return nullptr;
      }
      term_mtx.unlock();

      Thread t(handle_socket_helper, new int( sock ));
      t.detach();
   }
}

void* handle_socket_helper(void* sock_ptr) {
   ThreadLifeTracker tlt;

   FileDesc sock(* (int *) sock_ptr);
   delete (int *) sock_ptr;

   try {
      if (recv_int(sock) != VERFICATION_CODE)
         throw SocketException("Incorrect verfication code");

      while (true)  {
         char message_type = recv_char(sock);
         if (do_terminate)
            return nullptr;
         if (message_type == 'R') {
            handle_request(sock);
         } else if (message_type == 'S') {
            handle_send(sock);
            return nullptr;
         } else if (message_type == 'T')  {
            return nullptr;
         }
         else {
            throw SocketException( " got wrong message type" );
         }
      }
   } catch (SocketException& se) {
      std::cerr << "SocketException caught in handle_socket_helper. Error: " << se.what() << std::endl;
      return nullptr;
   }
}

// Given dynamically allocated socket (int) that is sending parsing pages
// delete will be called on the socket
// and the socket will be closed
void handle_send(int sock) {
   Vector<ParsedPage> pages = recv_parsed_pages(sock);
   Frontier::getFrontier().addUrls( std::move( pages ) );
}

// Given dynamically allocated socket (int) that is requesting more urls
// delete will be called on the socket
// and the socket will be closed
void handle_request(int sock) {
   Vector<SizeT> urls_to_parse = Frontier::getFrontier().getUrl();
   try {
      send_urls(sock, urls_to_parse);
   }
   catch( SocketException& se)
   {
      std::cerr << "SocketException in handle_request: " << se.what() << std::endl;
   }
}

int num_threads_alive() {
    fb::AutoLock l(term_mtx);
    return num_threads;
}
