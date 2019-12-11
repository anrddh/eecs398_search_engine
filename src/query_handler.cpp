#include <query/query_handler.hpp>
#include <query/query_result.hpp>
#include <fb/thread.hpp>
#include <disk/constants.hpp>
#include <algorithm>

//merge together the rank stats vector into the snippet
//for now, assuming the weight of each word is 1
//just to get something working quickly
//im literally just going to sort it for now lol
fb::Vector<fb::SizeT> MergeVectors(const fb::Vector<fb::Vector<uint32_t>> &occurrences){
    fb::Vector<fb::SizeT> vec;
    for (auto& v : occurrences){
        for (uint32_t i : v){
            vec.PushBack(i);
        }
    }
    std::sort(vec.begin(), vec.end());
    return vec;
}

void* RankPages( void *info ) {
   // Just keep calling add to top pages
   IndexInfoArg &arg = *(IndexInfoArg *) info;
   ConstraintSolver cSolver = arg.e->eval(sarg.reader);
   Vector<rank_stats> docsToRank = cSolver.GetDocumentsToRank();
   Vector<SizeT> docFreqs = cSolver.GetDocFrequencies();
   tfidf_rank(docsToRank, docFreqs);
   fb::Vector<QueryResult> qrs;
   for( rank_stats &stat : docsToRank )
      {
      QueryResult qr;
      qr.rank = stat.rank;
      fb::Vector<fb::SizeT> merged = MergeVectors(stat.occurrences);
      snip_window window = snippet_window_rank(merged, 150); //using 150 as max window size
      SnippetStats snip_stats = { PageStoreFile + toString(stat.page_store_number), stat.page_store_index, window };
      fb::Pair<fb::String, fb::String> SnipAndTit = GenerateSnippetsAndTitle(snip_stats, docsToRank);
      qr.UrlId = stat.UrlId;
      qr.Snippet = SnipAndTit.first;
      qr.Title = SnipAndTit.second;
      qrs.PushBack(qr);
      }
   return &qrs;
}

QueryHandler::QueryHandler( fb::String path , fb::String prefix, int num_indices ){
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

fb::Vector<QueryResult> QueryHandler::Query( fb::String q ) {
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
