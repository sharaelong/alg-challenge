#include <bits/stdc++.h>
using namespace std;

struct Graph {
    int V;
    vector<vector<int> > adj;
    bool is_query;

    vector<int> label;
    vector<bool> embedded;

    Graph(const string& filename, bool _is_query = false) {
        freopen(filename.c_str(), "r", stdin);

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
            }
        }

        for (int i=0; i<V; ++i) {
            sort(adj[i].begin(), adj[i].end());
        }

        // Maybe required...?
        if (is_query) {}

        printf("Graph entered\n");
    }

    bool is_neighbor(int u, int v) {
        if (adj[u].size() > adj[v].size()) { swap(u, v); }
        vector<int>::iterator it = lower_bound(adj[u].begin(), adj[u].end(), v);
        return (*it == v);
    }
};

struct CandidateSet {
    int V;
    vector<vector<int> > cs;

    CandidateSet(const string& filename) {
        freopen(filename.c_str(), "r", stdin);

        char type;
        scanf("%c %d\n", &type, &V);

        cs.resize(V);

        for (int i=0; i<V; ++i) {
            int query_vertex_id, size, data_vertex_id;
            scanf("%c %d %d ", &type, &query_vertex_id, &size);
            for (int j=0; j<size; ++j) {
                scanf("%d ", &data_vertex_id);
                cs[query_vertex_id].push_back(data_vertex_id);
            }
        }

        printf("candidateset entered\n");
    }
};

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Usage: ./program <data graph file> <query graph file> <candidate set file>\n");
        return EXIT_FAILURE;
    }

    string data_file_name = argv[1];
    string query_file_name = argv[2];
    string candidate_set_file_name = argv[3];

    printf("enter\n");

    Graph data(data_file_name);
    Graph query(query_file_name, true);
    CandidateSet candidate_set(candidate_set_file_name);

    // code here...
}
