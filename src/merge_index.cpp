#include "index/index_merger.hpp"
#include "fb/string.hpp"

int main(int argc, char ** argv)
   {
   if(argc < 3)
      {
         fb::String ErrorMessage = fb::String("Usage: ") + fb::String(argv[0]) + fb::String(" [files to merge ...] [output file]");
         std::cout << ErrorMessage << std::endl;
         exit(1);
      }

   fb::Vector<fb::String> inputFiles;
   for(int i = 1; i < argc - 1; ++i)
      {
      inputFiles.emplaceBack(argv[i]);
      }

   fb::String outputFile(argv[argc - 1]);
   IndexMerger(inputFiles, outputFile);

   }