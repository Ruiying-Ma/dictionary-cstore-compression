#include <bits/stdc++.h>
const int B = 3; // number of tuples per block. B > 1.
const int N = 5; // number of different symbols.
const int M = 2 * N; // number of different blocks.

struct Node;

struct Node {
    int seg_count = 0;
    int key;
    int value = -1; // value = -1 if the node hasn't been assigned any value
    std::map<Node*, int> neighbours; // (dest_node, weight)

    Node(int k): key(k) {}
};  

struct NodePtrCmp
{
    bool operator()(Node* const& u, Node* const& v) const
    {
        if (u->seg_count != v->seg_count) {
            return u->seg_count > v->seg_count;
        } else {
            return u->key < v->key;
        }
    }
};

// Sort the neighbours of `v` in decreasing order of `seg_count` for further assignment
// and enqueue them in order
// Then sort the neighbours of `v` in decreasing order of weight
void SortDesc(Node* v, std::queue<Node*>& cur_nodes, std::vector<std::pair<Node*, int>>& neighbours) {
    for (auto& u : v->neighbours) {
        if (u.first->value == -1) {
            neighbours.emplace_back(u.first, u.second);
        }
    }
    if (neighbours.size() == 0) {
        return;
    }
    std::sort(neighbours.begin(), neighbours.end(), 
                [](const std::pair<Node*, int>& u, const std::pair<Node*, int>& v) {return u.first->seg_count > v.first->seg_count;}
                );
    for (int i = 0; i < neighbours.size(); i++) {
        cur_nodes.push(neighbours[i].first);
    }
    std::sort(neighbours.begin(), neighbours.end(), 
                [](const std::pair<Node*, int>& u, const std::pair<Node*, int>& v) {return u.second > v.second;}
                );
}

void AssignValue(Node* node, int &left, int &right, int center = -1) {
    bool dirn;
    if (center == -1) {
        dirn = (left == 0) || ( (right < N - 1) && (N - right > left) );
    } else {
        dirn = (left == 0) || ( (right < N - 1) && (right - center < center - left) );
    }
    node->value = dirn ? ++right : --left;
    assert(node->value >= 0 && node->value < N);
    std::printf("Node %d is assigned with value %d\n", node->key, node->value);
}

void Sort(std::multiset<Node*, NodePtrCmp> nodes) { // the nodes are arranged in the decreasing order of seg_count
    int left = N / 2, right = N / 2 - 1;
    std::queue<Node*> q_nodes;
    while (!nodes.empty()) {
        Node* cur_node = *nodes.begin();
        q_nodes.push(cur_node);
        AssignValue(cur_node, left, right);
        nodes.erase(cur_node);
        while (!q_nodes.empty()) {
            cur_node = q_nodes.front();
            q_nodes.pop();
            std::vector<std::pair<Node*, int>> neighbours; // unassigned neighbours of `cur_node` in decreasing order of weights
            SortDesc(cur_node, q_nodes, neighbours);
            for (int i = 0; i < neighbours.size(); i++) { // assign these neighbours
                AssignValue(neighbours[i].first, left, right, cur_node->value);
                nodes.erase(neighbours[i].first);
            }
        }
    }
}

double SegMax(std::vector<int> &seg, std::vector<Node*> &mp) {
    int max = INT_MIN;
    for(int i = 1; i < B; i++) {
        int d = abs(mp[seg[i]]->value - mp[seg[0]]->value);
        if (max < d) {max = d;}
    }
    //std::printf("SegMax = %d\n", max);
    return (double)max;
}

double CalcNumBits(std::vector<Node*> &mp, std::vector<std::vector<int>>& segs) {
    double res = 0;
    for(int i = 0; i < M; i++) {
        double seg_max = SegMax(segs[i], mp);
        double num_bits = (seg_max == 0) ? 1 : (floor(log2(seg_max)) + 1);
        res += (segs[i][0] == 0) ? (double)1 : (floor(log2(segs[i][0])) + 1); // bits for `segs[i][0]`
        res += (double)(B-1) * num_bits;
    }
    return res;
}

/* Enumerate all possible assignments to find the optimal assignment */
double find_max(std::vector<int>& seg, std::vector<int>& dictionary) {
    int max = INT_MIN;
    for(int i = 1; i < B; i++) {
        int d = abs(dictionary[seg[i]] - dictionary[seg[0]]);
        if (max < d) {max = d;}
    }
    return (double)max;
}

double calc_num_bits(std::vector<int>& dictionary, std::vector<std::vector<int>>& segs) {
    double res = 0;
    for(int i = 0; i < M; i++) {
        double seg_max = find_max(segs[i], dictionary);
        double num_bits = (seg_max == 0) ? 1 : (floor(log2(seg_max)) + 1);
        res += (segs[i][0] == 0) ? (double)1 : (floor(log2(segs[i][0])) + 1); // bits for `segs[i][0]`
        res += (double)(B-1) * num_bits;
    }
    return res;
}

void print_perm(std::vector<int>& dictionary) {
    std::printf("\t The assignment is : ");
    for(auto& a : dictionary){std::printf("%d ", a);}
    std::printf("\n");
}

double enum_alg(std::vector<std::vector<int>>& segs, std::vector<int>& dictionary) {
    std::vector<int> temp_dic(N);
    for(int i = 0; i < N; i++){temp_dic[i] = i;}
    double min_cost = calc_num_bits(temp_dic, segs);
    while (std::next_permutation(temp_dic.begin(), temp_dic.end())) {
        int cost = calc_num_bits(temp_dic, segs);
        if (cost < min_cost) {
            min_cost = cost;
            dictionary = temp_dic;
        }
    }
    std::printf("enum_alg:\n");
    //print_perm(dictionary);
    std::printf("\t The cost is %lf bits\n", min_cost);
    return min_cost;
}

/* Random assignment */
double random_alg(std::vector<std::vector<int>>& segs, std::vector<int>& dictionary) {
    for(int i = 0; i < N; i++) {
        dictionary[i] = i;
    }
    unsigned seed = std::time(0);
    std::shuffle(dictionary.begin(), dictionary.end(), std::default_random_engine(seed));
    //std::printf("random_alg:\n");
    //print_perm(dictionary);
    //td::printf("\t The cost is %lf bits\n", calc_num_bits(dictionary, segs));
    return calc_num_bits(dictionary, segs);
}

double test() {
    // Construct the table
    std::vector<std::vector<int>> segs(M); // `M` segments, each containing B elements
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < B; j++) {
            segs[i].push_back(rand() % N);
        }
    }
    // Print the table
    for(auto& s : segs) {
        std::printf("[ ");
        for(int i = 0; i < B; i++) {
            std::printf("%d ", s[i]);
        }
        std::printf("]\n");
    }
    // Build the graph
    std::vector<Node*> vec_nodes(N);
    for(int i = 0; i < N; i++) {
        vec_nodes[i] = new Node(i);
    }
    std::vector<bool> rep(N);
    for(auto& s : segs) {
        Node* node = vec_nodes[s[0]]; // the first element of segment `s`
        node->seg_count++;
        for(int i = 0; i < N; i++){rep[i] = false;}
        for(int i = 1; i < B; i++) {
            if (!rep[s[i]] && s[i] != s[0]) {
                std::map<Node*, int>::iterator it = node->neighbours.find(vec_nodes[s[i]]);
                if (it != node->neighbours.end()) { // edge already appeared
                    node->neighbours[it->first]++;
                } else { // new edge
                    node->neighbours.emplace(vec_nodes[s[i]], 1);
                }
            }
            rep[s[i]] = true;
        }
    }
    std::multiset<Node*, NodePtrCmp> nodes; // arrange the nodes in decreasing order of `seg_count`
    for(int i = 0; i< N; i++) {
        nodes.insert(vec_nodes[i]);
    }
    assert(nodes.size() == N);
    // Print the graph
    for(auto it = nodes.begin(); it != nodes.end(); it++) {
        std::printf("Node %d has neighbours: ", (*it)->key);
        for(auto& e : (*it)->neighbours) {
            std::printf("(%d, %d) ", e.first->key, e.second);
        }
        std::printf("\n");
        std::printf("\tseg count: %d\n", (*it)->seg_count);
    }
    // Assigment
    Sort(nodes);
    // Print the nodes and calculate the cost
    //std::printf("sort: \n");
    // std::printf("\t The assignment is: ");
    // for(int i = 0; i < N; i++) {
    //     assert(vec_nodes[i] == vec_nodes[vec_nodes[i]->key]);
    //     assert(vec_nodes[i]->value >= 0 && vec_nodes[i]->value < N);
    //     std::printf("%d ", vec_nodes[i]->value);
    // }
    // std::printf("\n");
    //std::printf("\t The cost is %lf bits\n", CalcNumBits(vec_nodes, segs));
    double cost_sort = CalcNumBits(vec_nodes, segs);

    // Enum 
    std::vector<int> dictionary(N);
    //enum_alg(segs, dictionary);

    // Random
    double cost_rand = random_alg(segs, dictionary);

    return cost_rand / cost_sort; // return the compression rate
}

int main() {
    std::srand(std::time(0));
    std::fstream fs;
    fs.open("init_sol_data.txt", std::ios::out|std::ios::app);
    int tot = 1; // run `test()` for `tot` times
    for(int i = 0; i < tot; i++) {
        double res = test();
        fs<<res<<"\n";
    }
    fs.close();
    return 0;
}