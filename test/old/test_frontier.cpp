#include "Frontier.hpp"
#include "../../lib/thread.hpp"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace fb;

void* test_func(void* id_ptr) {
   cout << "inserting" << endl;
   for ( int i = 0; i < 30000; ++i ) {
      cout << "loop 1" << endl;
      int r = rand();
      FrontierUrl temp{r, r};
      cout << "loop 2" << endl;
      Frontier::getFrontier().addUrl(temp);
      cout << "loop 3" << endl;
   }
   cout << "finished inserting" << endl;
   int id = * (int*) id_ptr;
   cout << "calling getUrl" << endl;
   auto result1 = Frontier::getFrontier().getUrl();
   cout << "finished getUrl" << endl;
   for (auto i : result1) {
      cout << id << " " << i << endl;
   }
   cout << id << " got " << result1.size() << endl;
   cout << "second try for " << id  << endl;
   
   auto result2 = Frontier::getFrontier().getUrl();
   for (auto i : result2) {
      cout << id << " " << i << endl;
   }
   cout << id << " got " << result2.size() << endl;
   return nullptr;
}

int main() {
   int one = 1;
   int two = 2;
   Thread t1(test_func, &one);
   //Thread t2(test_func, &two);
   t1.join();
   //t2.join();
}
