// Created by Jaeyoon Kim 11/7/2019
#include "string_pool.hpp"
#include "vector.hpp"
#include <iostream>
#include <cassert>

using namespace std;

using namespace fb;

int main() {
   cout << -1 << endl;
   StringPool<16> pool("test_offset_file");
   std::cout << "first cout" << std::endl;
   SizeT offset = pool.get_offset("some url");
   std::cout << pool.access_offset(offset) << std::endl;
   pool.get_offset("good url");
   offset = pool.get_offset("good url");
   std::cout << pool.access_offset(offset) << std::endl;
   std::cout << pool.access_offset(offset) << std::endl;
   offset = pool.get_offset("bad url");
   SizeT offset2 = pool.get_offset("bad url");
   assert(offset == offset2);
   std::cout << pool.access_offset(offset) << std::endl;
}
