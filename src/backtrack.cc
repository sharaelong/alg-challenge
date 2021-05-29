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
  partial_embedding.reserve(query_num_vertices);
  partial_embedding_idx.reserve(query_num_vertices);

  is_matched.reserve(query_num_vertices);
  is_embedded.reserve(data->GetNumVertices());

  // initialize vectors to build a matching order
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched.push_back(false);
    partial_embedding_idx.push_back(-1);
  }

  BuildMatchingOrder();

  // reinitialize is_matched for embedding validation
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched[i] = false;
  }
  for (size_t i = 0; i < data->GetNumVertices(); ++i) {
    is_embedded.push_back(false);
  }
}

Backtrack::~Backtrack() {}

void Backtrack::PrintMatch() {
  std::cout << "a ";
  for (size_t i = 0; i < query->GetNumVertices(); ++i) {
    std::cout << partial_embedding[partial_embedding_idx[i]] << ' ';
  }
  std::cout << std::endl;
}

bool Backtrack::isValid(Vertex v) {
  if (is_embedded[v]) return false;
  size_t partial_embedding_size = partial_embedding.size();
  Vertex query_vertex = matching_order[partial_embedding_size];
  if (partial_embedding_size > 1) {
    for (size_t i = query->GetNeighborStartOffset(query_vertex); i < query->GetNeighborEndOffset(query_vertex); ++i) {
      if (is_matched[query->GetNeighbor(i)] && !data->IsNeighbor(v, partial_embedding[partial_embedding_idx[query->GetNeighbor(i)]])) {
        return false;
      } 
    }
  }
  return true;
}

void Backtrack::BuildMatchingOrder() {
  float curr_val, min_val;
  size_t query_num_vertices = query->GetNumVertices();
  size_t min_idx = 0, max_val = 0;

  for (size_t u = 0; u < query_num_vertices; ++u) {
    max_val = MAX(max_val, cs->GetCandidateSize(u));
  }

  min_val = (float)max_val; // Initialize min_val with the greatest value possible
  for (size_t u = 0; u < query_num_vertices; ++u) {
    curr_val = MIN(min_val, (float)cs->GetCandidateSize(u) / query->GetDegree(u));
    if (curr_val < min_val) {
      if (is_matched[u]) continue;
      min_val = curr_val;
      min_idx = u;
    }
  }
  is_matched[min_idx] = true;
  matching_order.push_back(min_idx);
  partial_embedding_idx[min_idx] = 0;
  for (size_t i = 0; i < query_num_vertices; ++i) {
    for (size_t u = 0; u < query_num_vertices; ++u) {
      if(!is_matched[u] && query->IsNeighbor(matching_order[i], u)) {
        is_matched[u] = true;
        matching_order.push_back(u);
        partial_embedding_idx[u] = matching_order.size() - 1;
      }
    }
  }
}

void Backtrack::CheckCandidateSpace() {
  Vertex v, u = matching_order[partial_embedding.size()];

  for (size_t i = 0; i < cs->GetCandidateSize(u); i++) {
    v = cs->GetCandidate(u, i);
    if (isValid(v)) {
      is_matched[u] = is_embedded[v] = true;
      partial_embedding.push_back(v);
      if (partial_embedding.size() == query->GetNumVertices())
        PrintMatch();
      else
        CheckCandidateSpace();
      partial_embedding.pop_back();
      is_matched[u] = is_embedded[v] = false;
    }
  }
}

void Backtrack::PrintAllMatches() {
  size_t query_num_vertices = query->GetNumVertices();
  std::cout << "t " << query_num_vertices << "\n";

  CheckCandidateSpace();
}