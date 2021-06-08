#include <bits/stdc++.h>
using namespace std;

const int INF = numeric_limits<int>::max();

struct SegmentTree {
    int size;
    vector<pair<int, int> > tree;
    SegmentTree() {}
    SegmentTree(int _size) {
        size = _size;
        tree.resize(4 * size);
        init(1, 0, size - 1);
    }

    void init(int node, int nl, int nr) {
        if (nl == nr) {
            tree[node] = { INF, nl };
            return;
        }
        int mid = (nl + nr) / 2;
        init(2 * node, nl, mid);
        init(2 * node + 1, mid + 1, nr);
        tree[node] = { INF, nl };
    }

    void modify(int node, int nl, int nr, int pos, int val) {
        if ((pos < nl) || (nr < pos)) { return; }
        if (nl == nr) {
            tree[node] = { val, pos };
            return;
        }
        int mid = (nl + nr) / 2;
        modify(2 * node, nl, mid, pos, val);
        modify(2 * node + 1, mid + 1, nr, pos, val);
        tree[node] = (tree[2 * node].first < tree[2 * node + 1].first ? tree[2 * node] : tree[2 * node + 1]);
    }
    
    void modify(int pos, int val) {
        modify(1, 0, size - 1, pos, val);
    }

    pair<int, int> query() {
        return tree[1];
    }
};

struct Graph {
    int V;
    vector<vector<int> > adj;
    bool is_query;
    int root_id; // query

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

        if (is_query) {
            prev.resize(V);
            next.resize(V);

            queue<int> bfs;
            vector<bool> visited(V, false);
            vector<bool> inqueue(V, false);
            bfs.push(0);
            root_id = 0;
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

struct CSNode {
    int id, weight, link_cnt;
    CSNode() {}
    CSNode(int _id, int _weight, int _link_cnt) {
        id = _id;
        weight = _weight;
        link_cnt = _link_cnt;
    }
};

void RemoveNode(vector<CSNode>& nodes, int vertex) {
    nodes.erase(remove_if(nodes.begin(), nodes.end(), [vertex](const CSNode& node) {
        return node.id == vertex;
    }), nodes.end());
}

struct CandidateSet {
    int V; // equal to query vertex count
    vector<vector<CSNode> > cs;
    vector<vector<int> > weight;

    CandidateSet() {}
    CandidateSet(const string& filename) {
        FILE* fp = freopen(filename.c_str(), "r", stdin);
        char type;
        scanf("%c %d\n", &type, &V);

        cs.resize(V);
        
        for (int i=0; i<V; ++i) {
            int query_vertex_id, size, graph_vertex_id;
            scanf("%c %d %d ", &type, &query_vertex_id, &size);
            
            for (int j=0; j<size; ++j) {
                scanf("%d ", &graph_vertex_id);
                cs[query_vertex_id].push_back({ graph_vertex_id, 1, 0 });
            }
        }
        fclose(fp);

        // fprintf(stderr, "Candidate set entered\n");
    }

    void preprocess(const Graph& graph, const Graph& query) {
        vector<int> multi(graph.V, 0); // multiplicity of vertex
        vector<vector<int> > used(graph.V); // vertex included cs
        queue<int> single; // 후보가 하나만 남은 candidate set
        
        for (int i=0; i<cs.size(); ++i) {
            for (int j=0; j<cs[i].size(); ++j) {
                int vertex = cs[i][j].id;
                used[vertex].push_back(i);
            }
            if (cs[i].size() == 1) {
                int tmp = cs[i][0].id;
                used[tmp].erase(remove(used[tmp].begin(), used[tmp].end(), i), used[tmp].end());
                single.push(i);
            }
        }

        while (!single.empty()) {
            int vertex = cs[single.front()][0].id;
            assert(cs[single.front()].size() == 1);
            single.pop();
            for (int i=0; i<used[vertex].size(); ++i) {
                int curr_set = used[vertex][i];
                RemoveNode(cs[curr_set], vertex);
                if (cs[curr_set].size() == 1) {
                    int removal = cs[curr_set][0].id;
                    used[removal].erase(remove(used[removal].begin(), used[removal].end(), curr_set), used[removal].end());
                    single.push(curr_set);
                }
            }
        }

        // // shuffle vertex
        // random_device rd;
        // mt19937 g(rd());
        // for (int i=0; i<V; ++i) {
        //     shuffle(cs[i].begin(), cs[i].end(), g);
        // }

        for (int i=0; i<cs.size(); ++i) {
            for (int j=0; j<cs[i].size(); ++j) {
                int vertex = cs[i][j].id;
                ++multi[vertex];
            }
        }

        for (int i=0; i<cs.size(); ++i) {
            stable_sort(cs[i].begin(), cs[i].end(), [&](const CSNode& a, const CSNode& b) {
                return (multi[a.id] / 8) < (multi[b.id] / 8);
            });
            
            // fprintf(stderr, "%d: ", cs[i].size());
            // for (int j=0; j<cs[i].size(); ++j) {
            //     fprintf(stderr, "(%d %d), ", cs[i][j].id, multi[cs[i][j].id]);
            // }
            // fprintf(stderr, "\n");
        }

        // // weight array
        // weight.resize(V);
        // for (int i=0; i<V; ++i) {
        //     weight[i].resize(cs[i].size(), 1);
        // }

        vector<int> outdegree(V);
        queue<int> q;
        for (int i=0; i<V; ++i) {
            outdegree[i] = query.next[i].size();
            if (outdegree[i] == 0) {
                q.push(i);
            }
        }
        
        while (!q.empty()) {
            int here_id = q.front();
            q.pop();
            for (int i=0; i<query.prev[here_id].size(); ++i) {
                int prev_id = query.prev[here_id][i];
                if (--outdegree[prev_id] == 0) {
                    q.push(prev_id);
                }
            }
            for (int i=0; i<cs[here_id].size(); ++i) {
                int here = cs[here_id][i].id;
                int min_weight = INF;
                for (int j=0; j<query.next[here_id].size(); ++j) {
                    int child_id = query.next[here_id][j];
                    if (query.prev[child_id].size() == 1) {
                        int weight_sum = 0;
                        for (int k=0; k<cs[child_id].size(); ++k) {
                            int child = cs[child_id][k].id;
                            if (graph.is_neighbor(here, child)) {
                                weight_sum += cs[child_id][k].weight;
                            }
                        }
                        min_weight = min(min_weight, weight_sum);
                    }
                }
                cs[here_id][i].weight = (min_weight == INF) ? 1 : min_weight;
            }
        }

        // for (int i=0; i<cs.size(); ++i) {
        //     fprintf(stderr, "%d: ", i);
        //     for (int j=0; j<cs[i].size(); ++j) {
        //         fprintf(stderr, "[%d %d] ", cs[i][j].id, cs[i][j].weight);
        //     }
        //     fprintf(stderr, "\n");
        // }

        // fprintf(stderr, "Preprocess of candidate set is finished\n");
    }
};

struct Backtrack {
    int V;
    Graph graph, query;
    CandidateSet candidate_set;

    vector<pair<int, int> > partial_embedding;
    vector<int> embedding;
    vector<int> indegree;
    vector<int> visited;
    SegmentTree segtree;
    vector<vector<int> > extendable_level;

    int matching_cnt = 0;

    Backtrack(const string& _data, const string& _query, const string& _candidate_set) {
        graph = Graph(_data);
        query = Graph(_query, true);
        candidate_set = CandidateSet(_candidate_set);
        candidate_set.preprocess(graph, query);
        
        V = query.V;
        embedding.resize(V, -1);
        indegree.resize(V);
        for (int i=0; i<V; ++i) {
            indegree[i] = query.prev[i].size();
        }
        visited.resize(graph.V, false);
        segtree = SegmentTree(V);
        extendable_level.resize(V);
    }

    void PrintMatching() {
        printf("a ");
        for (int i=0; i<V; ++i) {
            printf("%d ", embedding[i]);
        }
        printf("\n");

        if (++matching_cnt >= 100000) {
            exit(0);
        }
    }

    void PrintPartialMatching() {
        for (int i=0; i<partial_embedding.size(); ++i) {
            fprintf(stderr, "(%d %d) ", partial_embedding[i].first, partial_embedding[i].second);
        }
        fprintf(stderr, "\n");
    }

    pair<int, int> SelectExtendable(int size) {
        pair<int, int> result = segtree.query();
        if (size == 0) {
            result.second = query.root_id;
        }
        int add_id = result.second;
        segtree.modify(add_id, INF);
        for (int i=0; i<query.next[add_id].size(); ++i) {
            int next_id = query.next[add_id][i];
            if (--indegree[next_id] == 0) {
                extendable_level[size].push_back(next_id);
            }
        }
        return result;
    }

    void RemoveExtendable(int size, pair<int, int> poped) {
        int add_id = (size == 0 ? query.root_id : poped.second);
        for (int i=0; i<query.next[add_id].size(); ++i) {
            int next_id = query.next[add_id][i];
            ++indegree[next_id];
        }
        extendable_level[size].clear();
        segtree.modify(poped.second, poped.first);
    }

    void UpdateInfo(int size, bool add) {
        int here_id = partial_embedding.back().first;
        int here = partial_embedding.back().second;
        // update adjacent vertex link count
        for (int i=0; i<query.next[here_id].size(); ++i) {
            int next_id = query.next[here_id][i];
            int weight_sum = 0;
            for (int j=0; j<candidate_set.cs[next_id].size(); ++j) {
                CSNode& next = candidate_set.cs[next_id][j];
                // need to be improved
                if (graph.is_neighbor(here, next.id)) {
                    if (!add) { --next.link_cnt; }
                    else if (++next.link_cnt == query.prev[next_id].size()) {
                        weight_sum += next.weight;
                    }
                }
            }
            if (find(extendable_level[size].begin(), extendable_level[size].end(), next_id) != extendable_level[size].end()) {
                segtree.modify(next_id, add ? weight_sum : INF);
            }
        }
    }

    void TraverseCandidateSet() {
        int size = partial_embedding.size();
        if (size == V) {
            PrintMatching();
            return;
        }

        pair<int, int> poped = SelectExtendable(size);
        int here_id = poped.second;
        for (int i=0; i<candidate_set.cs[here_id].size(); ++i) {
            CSNode curr = candidate_set.cs[here_id][i];
            // fprintf(stderr, "(%d %d %d)\n", size, here_id, curr.id);
            if (curr.link_cnt == query.prev[here_id].size() && !visited[curr.id]) {
                partial_embedding.push_back({ here_id, curr.id });
                visited[curr.id] = true;
                embedding[here_id] = curr.id;
                UpdateInfo(size, true);
                
                // if (here_id == 1) { fprintf(stderr, "%d %d\n", curr.link_cnt, query.prev[here_id].size()); }
                // if (here_id == 1) {
                //     fprintf(stderr, "%d\n", extendable_level[size].size());
                //     fprintf(stderr, "%d %d\n", segtree.query().first, segtree.query().second);
                // }
                TraverseCandidateSet();

                UpdateInfo(size, false);
                embedding[here_id] = -1;
                visited[curr.id] = false;
                partial_embedding.pop_back();
            }
        }
        RemoveExtendable(size, poped);
    }
};

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./program <data graph file> <query graph file> <candidate set file>\n");
        return EXIT_FAILURE;
    }

    string data_file_name = argv[1];
    string query_file_name = argv[2];
    string candidate_set_file_name = argv[3];

    Backtrack backtrack(data_file_name, query_file_name, candidate_set_file_name);
    backtrack.TraverseCandidateSet();
}
