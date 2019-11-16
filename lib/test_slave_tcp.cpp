#include "slave_url_tcp.hpp"
#include "string.hpp"

using namespace fb;

int main(){
   int master_port = 8000; // HARD coding for testing
   String master_ip = "localhost";

   set_master_ip(master_ip, master_port);
	return 0;
}
