#include "frontier_pool.hpp"
#include <iostream>

using namespace std;

int main() {
   for ( int i = 0; i < 1000; ++i ) {
      add_url(i, i);
   }
   for (auto i : get_url()) {
      cout << i << endl;
   }
}
