// Created by Jaeyoon Kim 11/7/2019
#include "string_pool.hpp"
#include <vector>
#include <iostream>

using namespace std;
using namespace fb;

int main() {
   StringPool<16> pool("test_offset_file");
   SizeT offset = pool.get_offset("some url");
   cout << pool.access_offset(offset) << endl;
   pool.get_offset("good url");
   offset = pool.get_offset("good url");
   cout << pool.access_offset(offset) << endl;
   cout << pool.access_offset(offset) << endl;
   offset = pool.get_offset("bad url");
   SizeT offset2 = pool.get_offset("bad url");
   assert(offset = offset2);
   cout << pool.access_offset(offset) << endl;
}
