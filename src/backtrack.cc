/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack(const Graph *_data, const Graph *_query, const CandidateSet *_cs) {
  data = _data;
  query = _query;
  cs = _cs;
  size_t query_num_vertices = query->GetNumVertices();

  matching_order.reserve(query_num_vertices);
  candidate.reserve(query_num_vertices);
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched.push_back(false);
  }

  BuildMatchingOrder();
}

Backtrack::~Backtrack() {}

void Backtrack::PrintMatch() {
  std::cout << "a ";
  for (std::vector<Vertex>::const_iterator i = candidate.begin(); i != candidate.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;
}

bool Backtrack::isValid(Vertex v) {
  if (candidate.size() > 1) {
    bool is_valid = false;
    for (size_t i = 0; i < candidate.size() - 1; ++i) {
      if (query->IsNeighbor(matching_order.at(i), matching_order.at(candidate.size()))) {
        if(!(is_valid = data->IsNeighbor(v, candidate.at(i)))) return false;
      } 
    }
    // std::cout << is_valid << std::endl;
    return is_valid;
  }
  else return true;
}

void Backtrack::BuildMatchingOrder() {
  float curr_val, min_val;
  size_t query_num_vertices = query->GetNumVertices();
  size_t order_idx, min_idx = 0, max_val = 0;

  for (size_t u = 0; u < query_num_vertices; ++u) {
    max_val = MAX(max_val, cs->GetCandidateSize(u));
  }

  min_val = (float)max_val; // Initialize min_val with the greatest value possible
  for (size_t u = 0; u < query_num_vertices; ++u) {
    curr_val = MIN(min_val, (float)cs->GetCandidateSize(u) / query->GetDegree(u));
    if (curr_val < min_val) {
      if (is_matched.at(u)) continue;
      min_val = curr_val;
      min_idx = u;
    }
  }
  is_matched.at(min_idx) = true;
  matching_order.push_back(min_idx);
  
  order_idx = 0;
  while (order_idx < query_num_vertices) {
    for (size_t u = 0; u < query_num_vertices; ++u) {
      if(!is_matched.at(u) && query->IsNeighbor(matching_order.at(order_idx), u)) {
        is_matched.at(u) = true;
        matching_order.push_back(u);
      }
    }
    order_idx++;
  }
  // std::cout << "m ";
  // for (Vertex v: matching_order) {
  //   std::cout << v << ' ';
  // }
  // std::cout << std::endl;
}

void Backtrack::CheckCandidateSpace() {
  // printf("%-4ld", candidate.size());
  Vertex v, u = matching_order.at(candidate.size());

  for (size_t i = 0; i < cs->GetCandidateSize(u); i++) {
    v = cs->GetCandidate(u, i);
    if (isValid(v)) {
      candidate.push_back(v);
      if (candidate.size() == query->GetNumVertices())
        PrintMatch();
      else
        CheckCandidateSpace();
      candidate.pop_back();
    }
  }
}

void Backtrack::PrintAllMatches() {
  size_t query_num_vertices = query->GetNumVertices();
  std::cout << "t " << query_num_vertices << "\n";

  CheckCandidateSpace();
}