// Created by Jaeyoon Kim 11/6/2019
#include "saved_adjlist.hpp"
#include <vector>
#include <iostream>

using namespace std;
using namespace fb;

int main() {
   try {
   SavedAdjList file("test_file");
   vector<SizeT> test_adj_list_1 = {1, 11};
   vector<SizeT> test_adj_list_2 = {0};
   vector<SizeT> test_adj_list_3 = {7, 10, 1234};

   SizeT offset1 = file.add_adj_list( test_adj_list_1 );
   SizeT offset2 = file.add_adj_list(test_adj_list_2);
   SizeT offset3 = file.add_adj_list(test_adj_list_3);
   

   SizeT* val = file.get_adj_list(offset1);
   for (int i = 0; i < test_adj_list_1.size(); ++i) {
      assert(test_adj_list_1[i] == val[i]);
   }

   val = file.get_adj_list(offset2);
   for (int i = 0; i < test_adj_list_2.size(); ++i) {
      assert(test_adj_list_2[i] == val[i]);
   }

   val = file.get_adj_list(offset3);
   for (int i = 0; i < test_adj_list_3.size(); ++i) {
      assert(test_adj_list_3[i] == val[i]);
   }

   } catch ( Exception& e) {
      cout << e.msg << endl;
   }
}
