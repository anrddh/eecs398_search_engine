#pragma once

char * findSkipTable(char * start) 
   {
   while(*(start++) != '\0')
      ;
   start += (2 * sizeof(unsigned int));
   return start;
   }

int getHighestBit(int num)
   {
   int val = 0;
   while(num)
      {
      ++val;
      num >>= 1;
      }

   return val;
   }