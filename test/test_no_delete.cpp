#include <fb/no_delete_unordered_map.hpp>
#include <fb/string.hpp>
#include <iostream>
#include <cassert>
using namespace fb;

int main(int argc, char** argv)
{
    auto map = NoDeleteUnorderedMap<String, int>();
   int n = atoi(argv[1]);
   std::cout << "Inserting " << n << " values" << std::endl;
   for( int i = 0; i  < n; ++i)
   {
	   auto str = fb::toString(i);
	   map.insert(str, i);
   }

   std::cout << "Checking correctly inserted" <<  std::endl;
   for ( int i = 0; i < n; ++i )
   {
	auto str = fb::toString(i);
	auto iter = map.find(str);
	assert( iter != map.end() && *iter == i);
   }
   for ( int i = n; i < 2*n; ++i)
   {
       auto str = fb::toString(i);
       assert( map.find(str) == map.end() );
   }
   assert(map.size() == n);
   std::cout << "assert  passed!" <<  std::endl;
    return 0;
}
