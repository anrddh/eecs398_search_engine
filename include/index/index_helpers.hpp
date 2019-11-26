#pragma once

int getSizeOfSkipTable(int NUM_SKIP_TABLE_BITS) {
    return ((1 << NUM_SKIP_TABLE_BITS) * 2 * sizeof(unsigned int));
}

int getSizeOfRankingData() {
    return (2 * sizeof(unsigned int));
}

int getSizeOfPostingList(const fb::String &word, int NUM_SKIP_TABLE_BITS) {
    return (word.size() + 1) + getSizeOfRankingData() + getSizeOfSkipTable(NUM_SKIP_TABLE_BITS);
}

class DiskMapper 
   {
public:
   DiskMapper(int file, int offset, int size);
   ~DiskMapper();
   char * getPointer();

private:
   char *actualStart, *requestedStart;
   int actualSize, requestedSize;
   int actualOffset, requestedOffset;
   };

DiskMapper::DiskMapper(int file, int offset, int size) : requestedSize(size), requestedOffset(offset)
   {
   int page_size = sysconf(_SC_PAGE_SIZE);
   actualOffset = (offset / page_size) * page_size;
   int difference = requestedOffset - actualOffset;
   actualSize =  requestedSize + difference;

   actualStart = (char *) mmap(nullptr, actualSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, file, actualOffset);
   requestedStart = actualStart + difference;
   }

DiskMapper::~DiskMapper()
   {
   munmap(actualStart, actualSize);
   }

char * DiskMapper::getPointer()
   {
   return requestedStart;
   }

