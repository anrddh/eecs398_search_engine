#pragma once

#include "query_result.hpp"
#include "fb/thread.hpp"


struct IndexInfoArg {
    fb::UniquePtr<Expression> &e;
    fb::UniquePtr<IndexReader> &reader;
    TopPages &pages;
};

void* RankPages( void *info ) {
   // Just keep calling add to top pages
   IndexInfoArg &arg = *(IndexInfoArg *) info;
   ConstraintSolver cSolver = arg.e->eval(sarg.reader);
   Vector<rank_stats> docsToRank = cSolver.GetDocumentsToRank();
   Vector<SizeT> docFreqs = cSolver.GetDocFrequencies();
   tfidf_rank(docsToRank, docFreqs);
   for( rank_stats &stat : docsToRank )
      {
      QueryResult qr;
      qr.rank = stat.rank;
      
      arg.pages
      }
}

class QueryHandler 
   {
public:
   QueryHandler( fb::String path , fb::String prefix, int num_indices ) 
      {
      fb::String dirname(argv[1]);
      fb::String Prefix(argv[2]);
      int num_index_files = atoi(argv[3]);
      for (int i = 0; i < num_index_files; ++i) 
         {
         fb::String filename = dirname + "/" + Prefix + fb::toString(i);
         int f = open(filename.data(), O_RDWR);
         if(f < 0)
            {
            // write debug message
            std::cout << "ERROR OPENING FILE: " << filename << std::endl;
            exit(1);
            }

         struct stat details;
         fstat(f, &details);

         char *IndexPtr = (char *)mmap(nullptr, details.st_size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, f, 0);
         Readers.PushBack(fb::makeUnique<IndexReader>(IndexPtr, i));
         }

      
      }
   ~QueryHandler( )

   fb::Vector<QueryResult> Query( fb::String q )
      {
      TopPages pages(100);
      fb::UniquePtr<Expression> e = ParseQuery(q);
      fb::Vector<Thread> threads;
      for (auto& reader : Readers)
         {
         IndexInfoArg * info =  new IndexInfoArg{ e, reader, pages};
         threads.emplaceBack(RankPages, (void *)info);
         }

      for(auto& thread : threads)
         {
         thread.join( );
         }

      return pages.GetTopResults( );
      }
      
private:
   Vector<fb::UniquePtr<IndexReader>> Readers;

   }