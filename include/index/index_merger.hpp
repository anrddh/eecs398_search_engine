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
   IndexMerger(fb::Vector<fb::String> inputFiles, fb::String outputFile) : words_written(0)
      {
      // open files
      int counter = 0;
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
            IndexReaders.emplaceBack(start, counter);
            ++counter;
            }
         }

      // init table_size and IndexStarts
      IndexOffsetStarts.pushBack(0);
      table_size = 0;

      for(IndexReader &ir : IndexReaders)
         {
         table_size += ir.GetSizeOfTable( );
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
      ftruncate(MergedIndexFile, 4000000000);

      start = (char *) mmap(nullptr, 4000000000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, MergedIndexFile, 0);
   

      ((unsigned int*)start)[0] = 1;
      ((unsigned int*)start)[1] = IndexOffsetStarts.back( );
      ((unsigned int*)start)[2] = table_size;

      dictionary = ((unsigned int*)start) + 3;
      memset(dictionary, 0, table_size * 2 * sizeof(unsigned int));

      nextAvailableLocation = (table_size + 3) * sizeof(unsigned int);
      nextAvailableLocation += writeEODList( );
      int bucket = 0;
      while(bucket != -1)
         {
         bucket = processNextWord( bucket );
         assert(nextAvailableLocation >= (table_size + 3) * sizeof(unsigned int));
         }

      munmap(start, 4000000000);
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
         fb::UniquePtr<DocumentISR> docIsr = IndexReaders[j].OpenDocumentISR( );
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

   bool writePostingListLocation( fb::String &word )
      {
      fb::Hash<fb::String> hash;
      fb::SizeT bucket = hash(word) % table_size;
      fb::SizeT originalBucket = bucket;
      if(dictionary[bucket] != 0)
         {
         bucket = (bucket + 1) % table_size;
         while(dictionary[bucket] != 0)
            {
            bucket = (bucket + 1) % table_size;
            if(bucket == originalBucket)
               {
               perror("error empty bucket never found");
               }
            }
         }

      dictionary[bucket] = nextAvailableLocation;

      return true;
      }
   
   bool wordExists( fb::String &word )
      {
      fb::Hash<fb::String> hash;
      fb::SizeT bucket = hash(word) % table_size;
      fb::SizeT originalBucket = bucket;
      if(dictionary[bucket] == 0 || strcmp(start + dictionary[bucket], word.data() ) )
         {
         bucket = (bucket + 1) % table_size;
         while( originalBucket != bucket && dictionary[bucket] != 0 && strcmp( start + dictionary[bucket], word.data( ) ) )
            {
            bucket = (bucket + 1) % table_size;
            }
         }

         return dictionary[bucket] && !strcmp( start + dictionary[bucket], word.data( ) );
      }

   unsigned int writeWordList( fb::String &word )
      {
      if( wordExists( word ) ) 
         {
         std::cout << "duplicate word" << std::endl;
         exit(1);
         }
      writePostingListLocation(word);
      char * postingListLocation = start + nextAvailableLocation;
      int num_occurences = 0;
      int num_docs = 0;
      for(IndexReader &ir : IndexReaders)
         {
         fb::UniquePtr<WordISR> wordISR = ir.OpenPlainWordISR( word );
         num_occurences += wordISR->GetNumberOfOccurrences( );
         num_docs += wordISR->GetDocumentCount( );
         }
      
      int num_skip_table_bits = std::min( 10, std::max(1, getHighestBit( num_occurences ) - 6) );
      PostingListBuilder builder(word, postingListLocation, num_docs, num_occurences, MAX_TOKEN_BITS, num_skip_table_bits);
      uint64_t last = 0;
      for(fb::SizeT j = 0; j < IndexReaders.size( ); ++j)
         {
         fb::UniquePtr<WordISR> wordIsr= IndexReaders[j].OpenPlainWordISR( word );
         fb::UniquePtr<IndexInfo> info = wordIsr->GetCurrentInfo( );
         while(info)
            {
            uint64_t position = info->GetStartLocation( ) + IndexOffsetStarts[j];
            builder.addPost( AbsoluteWordInfo{ position - last, 0 } );
            last = position;
            info = wordIsr->Next( );
            }
         IndexReaders[j].deleteWord( word );
         assert( !IndexReaders[j].WordExists( word ) );
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
         ++words_written;
         return p.first + 1;
         }

      }

   unsigned int table_size;
   int MAX_TOKEN_BITS;
   uint64_t nextAvailableLocation;
   fb::Vector<IndexReader> IndexReaders;
   fb::Vector<uint64_t> IndexOffsetStarts;
   unsigned int * dictionary;
   int num_buckets;
   char * start;
   int words_written;

   };