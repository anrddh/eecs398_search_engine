#pragma once

#include <query/query_result.hpp>
#include <fb/thread.hpp>
#include <disk/constants.hpp>
#include <algorithm>

struct IndexInfoArg {
    fb::UniquePtr<Expression> &e;
    fb::UniquePtr<IndexReader> &reader;
    TopPages &pages;
};

//merge together the rank stats vector into the snippet
//for now, assuming the weight of each word is 1
//just to get something working quickly
//im literally just going to sort it for now lol
fb::Vector<fb::SizeT> MergeVectors(const fb::Vector<fb::Vector<uint32_t>> &occurrences);

<<<<<<< HEAD
void* RankPages( void *info ) {
   // Just keep calling add to top pages
   IndexInfoArg &arg = *(IndexInfoArg *) info;
   ConstraintSolver cSolver = arg.e->Constraints(arg.reader);
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
=======
class QueryHandler {
>>>>>>> db069194cd98dd5585f52599a831bb1a83949fac
public:
   QueryHandler( fb::String path , fb::String prefix, int num_indices );

   ~QueryHandler( )

   fb::Vector<QueryResult> Query( fb::String q );

private:
   Vector<fb::UniquePtr<IndexReader>> Readers;

};
