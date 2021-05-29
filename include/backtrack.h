/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#define MAX(num1, num2)   (num1)>(num2)?(num1):(num2)
#define MIN(num1, num2)   (num1)<(num2)?(num1):(num2)
#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
  public:
    explicit Backtrack(const Graph *_data, const Graph *_query, const CandidateSet *_cs);

    void PrintEmbedding();
    void BuildMatchingOrder();
    bool isValid(Vertex v);
    void CheckCandidateSpace();
    void PrintAllMatches();

  private:
    const Graph *data;
    const Graph *query;
    const CandidateSet *cs;

    std::vector<size_t> partial_embedding_idx;
    std::vector<Vertex> partial_embedding;
    std::vector<Vertex> matching_order;
    std::vector<bool>   is_matched;
    std::vector<bool>   is_embedded;
};

#endif  // BACKTRACK_H_
