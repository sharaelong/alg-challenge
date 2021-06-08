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

    int counter = 0;
    vector<int> discovered;
    vector<vector<int> > cut;
    vector<vector<int> > group;
    
    int find_bridge(int here, int from) {
        discovered[here] = ++counter;
        int ret = discovered[here];
        for (int i=0; i<adj[here].size(); ++i) {
            int there = adj[here][i];
            if (there == from) { continue; }
            if (discovered[there]) {
                ret = min(ret, discovered[there]);
                continue;
            }
            int child_dfs = find_bridge(there, here);
            ret = min(ret, child_dfs);
            if (child_dfs > discovered[here]) {
                cut[here].push_back(there);
                cut[there].push_back(here);
            }
        }
        return ret;
    }

    void decompose(int here) {
        discovered[here] = true;
        group[counter].push_back(here);
        for (int i=0; i<adj[here].size(); ++i) {
            int there = adj[here][i];
            if (!discovered[there] && find(cut[here].begin(), cut[here].end(), there) == cut[here].end()) {
                decompose(there);
            }
        }
    }

    // query graph의 order를 맞추는 과정
    // cs 내의 정점 순서를 맞추는 작업이 아니다
    void preprocess(vector<int>& embed_order) {
        counter = 0;
        discovered = vector<int>(V, 0);
        cut.resize(V);
        find_bridge(0, -1);

        counter = 0;
        discovered = vector<int>(V, false);
        group.resize(V);
        for (int i=0; i<V; ++i) {
            if (!discovered[i]) {
                decompose(i);
                ++counter;
            }
        }

        vector<double> density(counter);
        for (int i=0; i<counter; ++i) {
            int edge_cnt = 0;
            for (int j=0; j<group[i].size()-1; ++j) {
                for (int k=j+1; k<group[i].size(); ++k) {
                    if (is_neighbor(group[i][j], group[i][k])) { ++edge_cnt; }
                }
            }
            density[i] = (double)edge_cnt / group[i].size();
        }

        vector<int> group_idx(V);
        for (int i=0; i<counter; ++i) {
            for (int j=0; j<group[i].size(); ++j) {
                group_idx[group[i][j]] = i;
            }
        }

        embed_order.clear();
        embed_order.resize(V);
        for (int i=0; i<V; ++i) { embed_order[i] = i; }
        sort(embed_order.begin(), embed_order.end(), [&](int a, int b) {
            if (group_idx[a] == group_idx[b]) {
                return cut[a].size() > cut[b].size();
            } else {
                return density[group_idx[a]] > density[group_idx[b]];
            }
        });

        // debug
        fprintf(stderr, "bridge\n");
        for (int i=0; i<cut.size(); ++i) {
            for (int j=0; j<cut[i].size(); ++j) {
                fprintf(stderr, "%d ", cut[i][j]);
            }
            fprintf(stderr, "\n");
        }
        
        for (int i=0; i<counter; ++i) {
            fprintf(stderr, "group %d with size %d: ", i, group[i].size());
            for (int j=0; j<group[i].size(); ++j) {
                fprintf(stderr, "%d ", group[i][j]);
            }
            fprintf(stderr, "\n");
        }
        
        fprintf(stderr, "embed_order: ");
        for (int i=0; i<embed_order.size(); ++i) {
            fprintf(stderr, "%d ", embed_order[i]);
        }
        fprintf(stderr, "\n");
    }
};

struct CandidateSet {
    int V; // equal to query vertex count
    vector<vector<int> > cs;

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
                cs[query_vertex_id].push_back(graph_vertex_id);
            }
        }
        fclose(fp);

        // fprintf(stderr, "Candidate set entered\n");
    }

    void preprocess(const Graph& graph) {
        // shuffle vertex
        random_device rd;
        mt19937 g(rd());
        for (int i=0; i<V; ++i) {
            shuffle(cs[i].begin(), cs[i].end(), g);
        }
        
        vector<int> multi(graph.V, 0); // multiplicity of vertex
        vector<vector<int> > used(graph.V); // vertex included cs
        queue<int> single; // 후보가 하나만 남은 candidate set
        
        for (int i=0; i<cs.size(); ++i) {
            for (int j=0; j<cs[i].size(); ++j) {
                int vertex = cs[i][j];
                used[vertex].push_back(i);
            }
            if (cs[i].size() == 1) {
                used[cs[i][0]].erase(remove(used[cs[i][0]].begin(), used[cs[i][0]].end(), i), used[cs[i][0]].end());
                single.push(i);
            }
        }

        while (!single.empty()) {
            int vertex = cs[single.front()][0];
            assert(cs[single.front()].size() == 1);
            single.pop();
            for (int i=0; i<used[vertex].size(); ++i) {
                int curr_set = used[vertex][i];
                cs[curr_set].erase(remove(cs[curr_set].begin(), cs[curr_set].end(), vertex), cs[curr_set].end());
                if (cs[curr_set].size() == 1) {
                    int removal = cs[curr_set][0];
                    used[removal].erase(remove(used[removal].begin(), used[removal].end(), curr_set), used[removal].end());
                    single.push(curr_set);
                }
            }
        }

        for (int i=0; i<cs.size(); ++i) {
            for (int j=0; j<cs[i].size(); ++j) {
                int vertex = cs[i][j];
                ++multi[vertex];
            }
        }

        for (int i=0; i<cs.size(); ++i) {
            stable_sort(cs[i].begin(), cs[i].end(), [&](int a, int b) {
                if (cs[i].size() >= 100) {
                    return (multi[a] / 10) > (multi[b] / 10);
                } else if (cs[i].size() >= 20) {
                    return (multi[a] / 5) < (multi[b] / 5);
                } else {
                    return (multi[a] / 3) > (multi[b] / 3);
                }
            });
            
            // fprintf(stderr, "%d: ", cs[i].size());
            // for (int j=0; j<cs[i].size(); ++j) {
            //     fprintf(stderr, "(%d %d), ", cs[i][j], multi[cs[i][j]]);
            // }
            // fprintf(stderr, "\n");
        }
    }
};

Graph graph, query;
CandidateSet candidate_set;

vector<int> embedding;
vector<int> indegree;
vector<bool> data_used;

set<int> extendable_id;
vector<int> embed_order;

int dup_cnt = 0;
int nonedge_cnt = 0;
int reach_cnt = 0;

// check current embedded vertex has edges to "future" vertex here with here_id
bool is_embedding_valid(int here_id, int here) {
    // for (int i=0; i<query.prev[here_id].size(); ++i) {
    //     int prev_id = query.prev[here_id][i];
    //     // assert(embedding[prev_id] != -1);
    //     if (!graph.is_neighbor(here, embedding[prev_id])) {
    //         return false;
    //     }
    // }
    // return true;

    for (int i=0; i<query.adj[here_id].size(); ++i) {
        int adj_id = query.adj[here_id][i];
        if (embedding[adj_id] != -1 && !graph.is_neighbor(here, embedding[adj_id])) {
            return false;
        }
    }
    return true;
}

void TraverseCandidateSet(int embed_size) {
    // if (++reach_cnt > (1 << 19)) {
    //     return;
    // }

    // fprintf(stderr, "%d ", embed_size);
    
    if (embed_size == query.V) {
        // Print valid mapping
        printf("a ");
        for (int j=0; j<query.V; ++j) {
            printf("%d ", embedding[j]);
        }
        printf("\n");
        return;
    }

    // int here_id = *extendable_id.begin();
    // // printf("enter id %d at depth %d\n", here_id, embed_size);
    // extendable_id.erase(here_id);

    // for (int j=0; j<query.next[here_id].size(); ++j) {
    //     int next_id = query.next[here_id][j];
    //     if (--indegree[next_id] == 0) {
    //         extendable_id.insert(next_id);
    //     }
    // }

    int here_id = embed_order[embed_size];
    
    for (int i=0; i<candidate_set.cs[here_id].size(); ++i) {
        int here = candidate_set.cs[here_id][i];
        if (data_used[here]) {
            // ++dup_cnt;
            continue;
        }
        if (is_embedding_valid(here_id, here)) {
            data_used[here] = true;
            embedding[here_id] = here;
            ++embed_size;
        
            TraverseCandidateSet(embed_size);
            
            data_used[here] = false;
            embedding[here_id] = -1;
            --embed_size;
        } // else {
        //     ++nonedge_cnt;
        // }
    }

    // for (int j=0; j<query.next[here_id].size(); ++j) {
    //     int next_id = query.next[here_id][j];
    //     if (indegree[next_id]++ == 0) {
    //         extendable_id.erase(next_id);
    //     }
    // }

    // extendable_id.insert(here_id);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./program <data graph file> <query graph file> <candidate set file>\n");
        return EXIT_FAILURE;
    }

    string data_file_name = argv[1];
    string query_file_name = argv[2];
    string candidate_set_file_name = argv[3];

    graph = Graph(data_file_name);
    query = Graph(query_file_name, true);
    query.preprocess(embed_order);
    candidate_set = CandidateSet(candidate_set_file_name);

    candidate_set.preprocess(graph);

    embedding.resize(query.V, -1);
    indegree.resize(query.V, 0);
    data_used.resize(graph.V, false);
    for (int i=0; i<indegree.size(); ++i) {
        indegree[i] = query.prev[i].size();
        if (indegree[i] == 0) {
            extendable_id.insert(i);
        }
    }
    TraverseCandidateSet(0);
    
    // fprintf(stderr, "reach: %d\n", reach_cnt);
    // fprintf(stderr, "dup: %d, nonedge: %d\n", dup_cnt, nonedge_cnt);
}
