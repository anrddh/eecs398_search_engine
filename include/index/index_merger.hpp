#pragma once 

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <fcntl.h>

#include "index/posting_list_builder.hpp"
#include "index/index_data_structures.hpp"
#include "isr/index_reader.hpp"
#include "isr/word_isr.hpp"
#include "isr/document_isr.hpp"

class IndexMerger 
   {
public:
   IndexMerger(fb::Vector<fb::String> inputFiles, fb::String outputFile) 
      {
      // open files
      for(fb::String name : inputFiles)
         {
         int f = open((name).data(), O_RDWR);
         if(f < 0)
            {
            // write debug message
            std::cout << "ERROR OPENING FILE " << name << ". SKIPPING ..." << std::endl;
            }
         else
            {
            struct stat details;
            fstat(f, &details);
            const char * start = (const char *) mmap(nullptr, details.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, f, 0);
            close(f);
            IndexReaders.emplaceBack(start, 0);
            }
         }

      // init table_size and IndexStarts
      IndexOffsetStarts.pushBack(0);
      table_size = 0;

      for(IndexReader &ir : IndexReaders)
         {
         num_buckets += ir.GetSizeOfTable( );
         fb::UniquePtr<DocumentISR> doc = ir.OpenDocumentISR( );
         fb::UniquePtr<IndexInfo> oldInfo = doc->GetCurrentInfo( );
         fb::UniquePtr<IndexInfo> info = doc->Next( );
         while( info )
            {
            oldInfo = std::move( info );
            info = doc->Next( );
            }
         IndexOffsetStarts.pushBack( IndexOffsetStarts.back( ) + oldInfo->GetStartLocation( ) );
         }
      
      MAX_TOKEN_BITS = getHighestBit( IndexOffsetStarts.back( ) );
      
      // open MergedIndexFile
      int MergedIndexFile = open(outputFile.data( ), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
      ftruncate(MergedIndexFile, 400000000000);

      start = (char *) mmap(nullptr, 400000000000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, MergedIndexFile, 0);
   
      munmap(start, 400000000000);

      ((unsigned int*)start)[0] = 1;
      ((unsigned int*)start)[1] = IndexOffsetStarts.back( );
      ((unsigned int*)start)[2] = num_buckets;

      unsigned int * dictionary = ((unsigned int*)start) + 3;
      memset(dictionary, 0, table_size * 2 * sizeof(unsigned int));

      nextAvailableLocation = (table_size + 3) * sizeof(unsigned int);
      nextAvailableLocation += writeEODList( );
      int bucket = 0;
      while(bucket != -1)
         {
         bucket = processNextWord( bucket );
         }
      }

private:

   unsigned int writeEODList( )
      {
      char * postingListLocation = start + nextAvailableLocation;

      int num_docs = 0;
      fb::Vector<uint32_t> DocumentCounts;
      DocumentCounts.pushBack(0);
      
      for(IndexReader &ir : IndexReaders)
         {
         num_docs += ir.OpenDocumentISR( )->GetDocumentCount( );
         }
      int num_skip_table_bits = std::min( 10, std::max(1, getHighestBit( num_docs ) - 6) );

      PostingListBuilder builder( fb::String(), postingListLocation, num_docs, num_docs, MAX_TOKEN_BITS, num_skip_table_bits );
      uint64_t docNum = 0;
      for(fb::SizeT j = 0; j < IndexReaders.size( ); ++j)
         {
         fb::UniquePtr<DocumentISR> docIsr = IndexReaders[j].OpenDocumentISR( )->GetDocumentCount( );
         fb::UniquePtr<IndexInfo> info = docIsr->GetCurrentInfo( );
         while(info)
            {
            builder.addPost( DocIdInfo{ static_cast<unsigned int>(info->GetStartLocation( ) + IndexOffsetStarts[j]), docNum } );
            ++docNum;
            info = docIsr->Next( );
            }
         }

      builder.endEODList();
      return builder.getLength( ); 
      }

   unsigned int writeWordList( fb::String &word )
      {
      char * postingListLocation = start + nextAvailableLocation;
      int num_occurences = 0;
      int num_docs = 0;
      for(IndexReader &ir : IndexReaders)
         {
         fb::UniquePtr<WordISR> wordISR = ir.OpenWordISR( word );
         num_occurences += wordISR->GetNumberOfOccurrences( );
         num_docs += wordISR->GetDocumentCount( );
         }

      int num_skip_table_bits = std::min( 10, std::max(1, getHighestBit( num_occurences ) - 6) );
      PostingListBuilder builder(word, postingListLocation, num_docs, num_occurences, MAX_TOKEN_BITS, num_skip_table_bits);
      for(fb::SizeT j = 0; j < IndexReaders.size( ); ++j)
         {
         fb::UniquePtr<WordISR> wordIsr= IndexReaders[j].OpenWordISR( word );
         fb::UniquePtr<IndexInfo> info = wordIsr->GetCurrentInfo( );
         while(info)
            {
            builder.addPost( AbsoluteWordInfo{ info->GetStartLocation( ) + IndexOffsetStarts[j], 0 } );
            info = wordIsr->Next( );
            }
         IndexReaders[j].deleteWord( word );
         }

      builder.endList();
      return builder.getLength();
      }

   int processNextWord( int current_bucket )
      {
      if( IndexReaders.empty( ) )
         {
         return -1;
         }
      fb::Pair<int, fb::String> p = IndexReaders.back( ).GetNextWord( current_bucket );
      if(p.first == -1)
         {
         IndexReaders.popBack( );
         return 0;
         }
      else
         {
         nextAvailableLocation += writeWordList( p.second );
         return p.first + 1;
         }

      return true;
      }

   unsigned int table_size;
   int MAX_TOKEN_BITS;
   unsigned int nextAvailableLocation;
   fb::Vector<IndexReader> IndexReaders;
   fb::Vector<uint64_t> IndexOffsetStarts;
   int num_buckets;
   char * start;

   };