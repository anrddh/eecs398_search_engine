#include "fb/set.hpp"
#include "gtest/gtest.h"

#include <set>

#include <vector>

class SetTest : public ::testing::Test 
   {
   protected:
      void SetUp() override
         {
         a.insert(20);
         a.insert(10);
         a.insert(12);
         a.insert(8);
         a.insert(30);
         a.insert(25);
         a.insert(35);
         }

      fb::Set<int> a;
   };

TEST_F(SetTest, BasicIteratorTest)
   {
   fb::Set<int>::Iterator iter = a.begin();
   ASSERT_EQ(*iter, 8);
   ++iter;
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 12);
   ++iter;
   ASSERT_EQ(*iter, 20);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());
   }
   
TEST_F(SetTest, FindTest)
   {
   fb::Set<int>::Iterator iter = a.find(20);
   ASSERT_NE(iter, a.end());
   ASSERT_EQ(*iter, 20);
   ++iter;
   ASSERT_EQ(*iter, 25);

   iter = a.find(23);
   ASSERT_EQ(iter, a.end());
   }

TEST_F(SetTest, EraseTest)
   {
   a.erase(20);
   fb::Set<int>::Iterator iter = a.begin();
   ASSERT_EQ(*iter, 8);
   ++iter;
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 12);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());

   a.erase(8);

   iter = a.begin();
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 12);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());

   a.erase(30);

   iter = a.begin();
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 12);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());

   a.erase(35);

   iter = a.begin();
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 12);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(iter, a.end());
   }

TEST_F(SetTest, InsertAndErase)
   {
   a.erase(8);
   a.erase(20);
   a.erase(12);
   a.insert(-8);
   a.insert(34);

   fb::Set<int>::Iterator iter = a.begin();
   ASSERT_EQ(*iter, -8);
   ++iter;
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 34);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());

   a.insert(27);
   a.insert(14);

   iter = a.begin();
   ASSERT_EQ(*iter, -8);
   ++iter;
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 14);
   ++iter;
   ASSERT_EQ(*iter, 25);
   ++iter;
   ASSERT_EQ(*iter, 27);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 34);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());

   a.erase(34);
   a.erase(25);

   iter = a.begin();
   ASSERT_EQ(*iter, -8);
   ++iter;
   ASSERT_EQ(*iter, 10);
   ++iter;
   ASSERT_EQ(*iter, 14);
   ++iter;
   ASSERT_EQ(*iter, 27);
   ++iter;
   ASSERT_EQ(*iter, 30);
   ++iter;
   ASSERT_EQ(*iter, 35);
   ++iter;
   ASSERT_EQ(iter, a.end());
   }
