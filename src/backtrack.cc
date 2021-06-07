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

void Backtrack::GenerateRandomOrder() {
  size_t query_num_vertices = query->GetNumVertices();
  std::vector<std::vector<Vertex>> neighbors(query_num_vertices, std::vector<Vertex>());
  Vertex first;
  
  for (size_t i = 0; i < query_num_vertices; ++i) is_matched[i] = false;
  
  first = matching_order[0];
  matching_order.clear();
  matching_order.push_back(first);
  partial_embedding_idx[first] = 0;
  is_matched[first] = true;

  for (size_t i = 0; i < query_num_vertices; ++i) {
    size_t curr_idx = matching_order[i];
  
    for (size_t u = query->GetNeighborStartOffset(curr_idx); u < query->GetNeighborEndOffset(curr_idx); ++u) {
      Vertex v = query->GetNeighbor(u);
      if(!is_matched[v]) neighbors[i].push_back(v);
    }
    std::random_shuffle(neighbors[i].begin(), neighbors[i].end());
    for (Vertex v : neighbors[i]) {
      is_matched[v] = true;
      matching_order.push_back(v);
      partial_embedding_idx[v] = matching_order.size() - 1;
    }
  }
  for (size_t i = 0; i < query_num_vertices; ++i) {
    is_matched[i] = false;
  }
}

/**
 * @brief Is called recursively, randomly searcing for a partial embedding.
 *        Returns true if an embedding is found.
 */
bool Backtrack::CheckRandomEmbedding() {
  if (++embedding_check_count >= EM_RAND_LIMIT) return false;
  bool ret = false;
  Vertex v, u = matching_order[partial_embedding.size()];
  v = cs->GetCandidate(u, rand() % cs->GetCandidateSize(u));
  if (isValid(v)) {
    is_matched[u] = is_embedded[v] = true;
    partial_embedding.push_back(v);
    if (partial_embedding.size() == query->GetNumVertices()) {
      ret = true;
      printf("WERE SAVED\n");
    }
    else
      ret = CheckRandomEmbedding();
    partial_embedding.pop_back();
    is_matched[u] = is_embedded[v] = false;
  }
  return ret;
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

  /*
   * Version 3
   */
  for (int i = 0; i < MO_RAND_LIMIT; ++i) {
    embedding_check_count = 0;
    GenerateRandomOrder();
    if(CheckRandomEmbedding()) break;
  }
  if (embedding_check_count >= EM_RAND_LIMIT) printf("WERE FUCKED\n");
  /*
   * Version 2
   */
  // for (size_t i = 0; matching_order.size() < query_num_vertices;) {
  //   size_t curr_idx = matching_order[i];
  //   min_val = (float)data->GetNumVertices(); // Initialize min_val with a sufficiently large value
  //   match_found = false;
  // 
  //   for (size_t u = query->GetNeighborStartOffset(curr_idx); u < query->GetNeighborEndOffset(curr_idx); ++u) {
  //     Vertex v = query->GetNeighbor(u);
  //     curr_val = MIN(min_val, (float)cs->GetCandidateSize(v) / query->GetDegree(v));
  //     if (curr_val < min_val && !is_matched[v]) {
  //       match_found = true;
  //       min_val = curr_val;
  //       min_idx = v;
  //     }
  //   }
  // 
  //   if (match_found) {
  //     is_matched[min_idx] = true;
  //     matching_order.push_back(min_idx);
  //     partial_embedding_idx[min_idx] = matching_order.size() - 1;
  //   }
  //   else i++;
  // }

  /*
   *  Version 1
   */
  // for (size_t i = 0; i < query_num_vertices; ++i) {
  //   for (size_t u = 0; u < query_num_vertices; ++u) {
  //     if(!is_matched[u] && query->IsNeighbor(matching_order[i], u)) {
  //       is_matched[u] = true;
  //       matching_order.push_back(u);
  //       partial_embedding_idx[u] = matching_order.size() - 1;
  //     }
  //   }
  // }
}

/**
 * @brief Is called recursively, building a partial embedding.
 *        Prints embedding if a complete embedding is found. 
 */
void Backtrack::CheckCandidateSpace() {
  Vertex v, u = matching_order[partial_embedding.size()];

  for (size_t i = 0; i < cs->GetCandidateSize(u); i++) {
    v = cs->GetCandidate(u, i);
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