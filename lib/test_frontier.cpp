#include "frontier_pool.hpp"
#include "thread.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace fb;

void* test_func(void* id_ptr) {
   for ( int i = 0; i < 30000; ++i ) {
      int r = rand();
      frontier_add_url(r, r);
   }
   int id = * (int*) id_ptr;
   auto result1 = frontier_get_url();
   for (auto i : result1) {
      //cout << id << " " << i << endl;
   }
   cout << id << " got " << result1.size() << endl;
   cout << "second try for " << id  << endl;
   
   auto result2 = frontier_get_url();
   for (auto i : result2) {
      //cout << id << " " << i << endl;
   }
   cout << id << " got " << result2.size() << endl;
   return nullptr;
}

int main() {
   int one = 1;
   int two = 2;
   Thread t1(test_func, &one);
   Thread t2(test_func, &two);
   t1.join();
   t2.join();
}
