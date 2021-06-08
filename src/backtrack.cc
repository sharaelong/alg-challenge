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
  size_t data_num_vertices = data->GetNumVertices();
  matching_order.reserve(query_num_vertices);
  partial_embedding.reserve(query_num_vertices);
  partial_embedding_idx.reserve(query_num_vertices);

  is_matched.reserve(query_num_vertices);
  is_embedded.reserve(data_num_vertices);

  // initialize vectors to build a matching order
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched.push_back(false);
    partial_embedding_idx.push_back(-1);
  }
  for (size_t i = 0; i < data_num_vertices; ++i) {
    is_embedded.push_back(false);
  }

  BuildMatchingOrder();

  // reinitialize vectors for embedding validation
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched[i] = false;
  }
  for (size_t i = 0; i < data_num_vertices; ++i) {
    is_embedded[i] = false;
  }
}

/**
 * @brief Prints the found embedding.
 */
void Backtrack::PrintEmbedding() {
  std::cout << "a ";
  for (size_t i = 0; i < query->GetNumVertices(); ++i) {
    std::cout << partial_embedding[partial_embedding_idx[i]] << ' ';
  }
  std::cout << std::endl;
}

/**
 * @brief Checks if the query vertex containing the given data vertex is a valid extendable candidate.
 * 
 * @param v data vertex to check
 * @retval true if valid, false if not.
 */
bool Backtrack::isValid(Vertex v) {
  if (is_embedded[v]) return false;
  size_t partial_embedding_size = partial_embedding.size();
  Vertex query_vertex = matching_order[partial_embedding_size];
  if (partial_embedding_size) {
    for (size_t i = query->GetNeighborStartOffset(query_vertex); i < query->GetNeighborEndOffset(query_vertex); ++i) {
      if (is_matched[query->GetNeighbor(i)] && !data->IsNeighbor(v, partial_embedding.at(partial_embedding_idx[query->GetNeighbor(i)]))) {
        return false;
      } 
    }
  }
  return true;
}

/**
 * @brief Builds a matching order of the query graph.
 */
void Backtrack::BuildMatchingOrder() {
  float curr_val, min_val;
  bool match_found;
  size_t query_num_vertices = query->GetNumVertices();
  size_t min_idx = 0;
  std::srand (unsigned(std::time(0)));

  min_val = (float)data->GetNumVertices(); // Initialize min_val with a sufficiently large value

  for (size_t u = 0; u < query_num_vertices; ++u) {
    curr_val = MIN(min_val, (float)cs->GetCandidateSize(u) / query->GetDegree(u));
    if (curr_val < min_val && !is_matched[u]) {
      min_val = curr_val;
      min_idx = u;
    }
  }
  
  is_matched[min_idx] = true;
  matching_order.push_back(min_idx);
  partial_embedding_idx[min_idx] = 0;

  for (size_t i = 0; matching_order.size() < query_num_vertices;) {
    size_t curr_idx = matching_order[i];
    min_val = (float)data->GetNumVertices(); // Initialize min_val with a sufficiently large value
    match_found = false;
  
    for (size_t u = query->GetNeighborStartOffset(curr_idx); u < query->GetNeighborEndOffset(curr_idx); ++u) {
      Vertex v = query->GetNeighbor(u);
      curr_val = MIN(min_val, (float)cs->GetCandidateSize(v) / query->GetDegree(v));
      if (curr_val < min_val && !is_matched[v]) {
        match_found = true;
        min_val = curr_val;
        min_idx = v;
      }
    }
  
    if (match_found) {
      is_matched[min_idx] = true;
      matching_order.push_back(min_idx);
      partial_embedding_idx[min_idx] = matching_order.size() - 1;
    }
    else i++;
  }
}

/**
 * @brief Is called recursively, building a partial embedding.
 *        Prints embedding if a complete embedding is found. 
 */
void Backtrack::CheckCandidateSpace() {
  Vertex v, u = matching_order[partial_embedding.size()];
  std::vector<std::pair<size_t, size_t>> candidate_entry;
  for (size_t i = 0; i < cs->GetCandidateSize(u); i++) {
    v = cs->GetCandidate(u, i);
    candidate_entry.push_back(std::make_pair(data->GetNeighborEndOffset(v)-data->GetNeighborStartOffset(v)+1, i));
  }
  std::sort(candidate_entry.begin(), candidate_entry.end());
  for (std::pair<size_t, size_t> i: candidate_entry) {
    v = cs->GetCandidate(u, i.second);
    if (isValid(v)) {
      is_matched[u] = is_embedded[v] = true;
      partial_embedding.push_back(v);
      if (partial_embedding.size() == query->GetNumVertices())
        PrintEmbedding();
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