// Created by Jaeyoon Kim 11/6/2019
#include "SavedLists.hpp"
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;
using namespace fb;

int main() {
   try {
   SavedLists<SizeT> file("test_file");
   vector<SizeT> test_list_1 = {1, 11};
   vector<SizeT> test_list_2 = {0};
   vector<SizeT> test_list_3 = {7, 10, 1234};

   SizeT offset1 = file.add_list( test_list_1 );
   SizeT offset2 = file.add_list( test_list_2);
   SizeT offset3 = file.add_list( test_list_3);

   SizeT* val = file.get_list(offset1);
   for (int i = 0; i < test_list_1.size(); ++i) {
      assert(test_list_1[i] == val[i]);
   }

   val = file.get_list(offset2);
   for (int i = 0; i < test_list_2.size(); ++i) {
      assert(test_list_2[i] == val[i]);
   }

   val = file.get_list(offset3);
   for (int i = 0; i < test_list_3.size(); ++i) {
      assert(test_list_3[i] == val[i]);
   }

   } catch ( Exception& e) {
      cout << e.msg << endl;
   }
}
