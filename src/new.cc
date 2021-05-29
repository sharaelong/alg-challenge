#include <bits/stdc++.h>
using namespace std;

struct Graph {
    int V;
    vector<vector<int> > adj;
    bool is_query;

    vector<int> label;
    vector<bool> embedded;

    vector<vector<int> > prev;
    vector<vector<int> > next;
    vector<pair<int, int> > edges;

    Graph() {}
    Graph(const string& filename, bool _is_query = false) {
        FILE* fp = freopen(filename.c_str(), "r", stdin);

        char type;
        int graph_id;
        scanf("%c %d %d\n", &type, &graph_id, &V);
        is_query = _is_query;

        adj.resize(V);
        label.resize(V);
        embedded.resize(V, false);

        int vertex_id1, vertex_id2, vertex_label;
        while (scanf("%c ", &type) != -1) {
            if (type == 'v') {
                scanf("%d %d\n", &vertex_id1, &vertex_label);
                label[vertex_id1] = vertex_label;
            } else {
                scanf("%d %d 0\n", &vertex_id1, &vertex_id2);
                adj[vertex_id1].push_back(vertex_id2);
                adj[vertex_id2].push_back(vertex_id1);
                edges.push_back(make_pair(vertex_id1, vertex_id2));
            }
        }

        fclose(fp);

        for (int i=0; i<V; ++i) {
            sort(adj[i].begin(), adj[i].end());
        }

        // Maybe required...?
        if (is_query) {
            prev.resize(V);
            next.resize(V);

            queue<int> bfs;
            vector<bool> visited(V, false);
            vector<bool> inqueue(V, false);
            bfs.push(0);
            inqueue[0] = true;

            while (!bfs.empty()) {
                int here = bfs.front();
                bfs.pop();
                visited[here] = true;
                inqueue[here] = false;

                for (int i=0; i<adj[here].size(); ++i) {
                    int there = adj[here][i];
                    if (!visited[there]) {
                        if (!inqueue[there]) {
                            bfs.push(there);
                            inqueue[there] = true;
                        }
                        next[here].push_back(there);
                    } else {
                        prev[here].push_back(there);
                    }
                }
            }
        }

        // fprintf(stderr, "Graph entered\n");
    }

    bool is_neighbor(int u, int v) const {
        if (adj[u].size() > adj[v].size()) { swap(u, v); }
        vector<int>::const_iterator it = lower_bound(adj[u].begin(), adj[u].end(), v);
        return (*it == v);
    }
};

struct CandidateSet {
    int V; // equal to query vertex count
    int cs_V; // number of kind of vertex in candidate set
    vector<vector<int> > cs;
    vector<vector<int> > adj; // It only stores adjacent information of adjacent vertex in query graph

    CandidateSet() {}
    CandidateSet(const string& filename, const Graph& data, const Graph& query) {
        FILE* fp = freopen(filename.c_str(), "r", stdin);
        char type;
        scanf("%c %d\n", &type, &V);

        vector<int> cand_included;
        cs.resize(V);
        for (int i=0; i<V; ++i) {
            int query_vertex_id, size, data_vertex_id;
            scanf("%c %d %d ", &type, &query_vertex_id, &size);
            for (int j=0; j<size; ++j) {
                scanf("%d ", &data_vertex_id);
                cs[query_vertex_id].push_back(data_vertex_id);
                cand_included.push_back(data_vertex_id);
            }
        }
        fclose(fp);

        // 좌표압축, candidate set 내의 정점에만 indexing
        sort(cand_included.begin(), cand_included.end());
        cand_included.erase(unique(cand_included.begin(), cand_included.end()), cand_included.end());
        cs_V = cand_included.size();

        vector<int> mapping(data.V);
        for (int i=0; i<cand_included.size(); ++i) {
            mapping[cand_included[i]] = i;
        }

        // candidate set의 정점들만으로 자체적인 graph 생성
        adj.resize(cs_V);
        for (int k=0; k<query.edges.size(); ++k) {
            int u = query.edges[k].first;
            int v = query.edges[k].second;
            for (int i=0; i<cs[u].size(); ++i) {
                for (int j=0; j<cs[v].size(); ++j) {
                    if (data.is_neighbor(cs[u][i], cs[v][j])) {
                        adj[mapping[cs[u][i]]].push_back(mapping[cs[v][j]]);
                        adj[mapping[cs[v][j]]].push_back(mapping[cs[u][i]]);
                    }
                }
            }
        }
        // erase duplication
        for (int i=0; i<cs_V; ++i) {
            sort(adj[i].begin(), adj[i].end());
            adj[i].erase(unique(adj[i].begin(), adj[i].end()), adj[i].end());
        }

        // change index of candidate set list
        for (int i=0; i<V; ++i) {
            for (int j=0; j<cs[i].size(); ++j) {
                cs[i][j] = mapping[cs[i][j]];
            }
        }

        // fprintf(stderr, "Candidate set entered\n");
    }

    bool is_neighbor(int u, int v) {
        vector<int>::iterator it = lower_bound(adj[u].begin(), adj[u].end(), v);
        return (*it == v);
    }
};

Graph data, query;
CandidateSet candidate_set;

vector<int> embedding;
vector<int> indegree;
set<int> extendable_id;
vector<bool> data_used;

// check current embedded vertex has edges to "future" vertex here with here_id
bool is_embedding_valid(int here_id, int here) {
    for (int i=0; i<query.prev[here_id].size(); ++i) {
        int prev_id = query.prev[here_id][i];
        assert(embedding[prev_id] != -1);
        if (!candidate_set.is_neighbor(here, embedding[prev_id])) {
            return false;
        }
    }
    return true;
}

void TraverseCandidateSet(int embed_size) {
    if (embed_size == query.V) {
        // // Validation
        // for (int i=0; i<query.V; ++i) {
        //     // Hmm?
        //     assert(data.label[embedding[i]] == query.label[i]);
        // }
        // for (int i=0; i<query.edges.size(); ++i) {
        //     int u_id = query.edges[i].first;
        //     int v_id = query.edges[i].second;
        //     assert(candidate_set.is_neighbor(embedding[u_id], embedding[v_id]));
        // }

        // Print valid mapping
        printf("a ");
        for (int j=0; j<query.V; ++j) {
            printf("%d ", embedding[j]);
        }
        printf("\n");
        return;
    }

    int here_id = *extendable_id.begin();
    // printf("enter id %d at depth %d\n", here_id, embed_size);
    extendable_id.erase(here_id);

    for (int j=0; j<query.next[here_id].size(); ++j) {
        int next_id = query.next[here_id][j];
        if (--indegree[next_id] == 0) {
            extendable_id.insert(next_id);
        }
    }
    
    for (int i=0; i<candidate_set.cs[here_id].size(); ++i) {
        int here = candidate_set.cs[here_id][i];
        if (data_used[here]) { continue; }
        if (is_embedding_valid(here_id, here)) {
            data_used[here] = true;
            embedding[here_id] = here;
            ++embed_size;
        
            TraverseCandidateSet(embed_size);
            
            data_used[here] = false;
            embedding[here_id] = -1;
            --embed_size;
        }
    }

    for (int j=0; j<query.next[here_id].size(); ++j) {
        int next_id = query.next[here_id][j];
        if (indegree[next_id]++ == 0) {
            extendable_id.erase(next_id);
        }
    }

    extendable_id.insert(here_id);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./program <data graph file> <query graph file> <candidate set file>\n");
        return EXIT_FAILURE;
    }

    string data_file_name = argv[1];
    string query_file_name = argv[2];
    string candidate_set_file_name = argv[3];

    // fprintf(stderr, "enter\n");

    data = Graph(data_file_name);
    query = Graph(query_file_name, true);
    candidate_set = CandidateSet(candidate_set_file_name, data, query);

    // code here...
    data_used.resize(candidate_set.cs_V, false);
    embedding.resize(query.V, -1);
    indegree.resize(query.V);
    for (int i=0; i<indegree.size(); ++i) {
        indegree[i] = query.prev[i].size();
        if (indegree[i] == 0) {
            extendable_id.insert(i);
        }
    }

    // fprintf(stderr, "%s", "Process start.\n");
    TraverseCandidateSet(0);
}
