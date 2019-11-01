#include "indexEntry.hpp"
#include <iostream>
#include <iomanip>

using namespace fb;
using namespace std;

int main() {
   char* original = (char*) new uint64_t[16];
   char* mem = original;
   mem = add_num(mem, 3, BOLD_FLAG | ITALICS_FLAG);
   mem = add_num(mem, 1623, ITALICS_FLAG);
   mem = add_num(mem, 610516583293);

   uint64_t value;
   uint8_t header;
   mem = original;
   mem = read_number(mem, value, header);
   cout << int(header) << " " << value << endl;
   mem = read_number(mem, value, header);
   cout << int(header) << " " << value << endl;
   mem = read_number(mem, value, header);
   cout << int(header) << " " << value << endl;
}
