#include <fb/no_delete_unordered_map.hpp>
#include <fb/string.hpp>
#include <iostream>
using namespace fb;

int main()
{
    auto map = NoDeleteUnorderedMap<String, int>();
    map["asdas"];
   auto x =  map.find("asdas");
   String a = "a";
   auto k = map.functionThatIsOnlyForJIaeyoonInThatOneSpecialCase(a);
   std::cout << map.size() << std::endl;
    return 0;
}
