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
    ~Backtrack();

    void PrintMatch();
    void BuildMatchingOrder();
    bool isValid(Vertex v);
    void CheckCandidateSpace();
    void PrintAllMatches();

  private:
    const Graph *data;
    const Graph *query;
    const CandidateSet *cs;

    std::vector<Vertex> candidate;
    std::vector<Vertex> matching_order;
    std::vector<bool>   is_matched;
};

#endif  // BACKTRACK_H_
