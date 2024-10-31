#include <bits/stdc++.h>
int B = 2; // number of tuples per block
int N = 1000; // number of different symbols
int M = 2 * N; // number of different blocks

void print_perm(std::vector<int>& dictionary) {
    std::printf("\t The permutation is : ");
    for(auto& a : dictionary){std::printf("%d ", a);}
    std::printf("\n");
}

/* Compute the approx number of bits used after compression */
double compression(double max_range) {
    return (double)M * log(N) + (double)M * (double)(B - 1) * log(max_range);
}

/* Find the maximum diameter of blocks under certain assigment */
int find_max(std::vector<std::pair<int, int>>& blocks, std::vector<int>& dictionary) {
    int max_range = 0;
    for (auto& a : blocks) {
        int cur_range = abs(dictionary[a.first] - dictionary[a.second]);
        if (cur_range > max_range) {max_range = cur_range;}
    }
    return max_range;
}


/* Enumerate all possible assignments to find the optimal assignemt */
int enum_alg(std::vector<std::pair<int, int>>& blocks, std::vector<int>& dictionary) {
    std::vector<int> temp_dic(N);
    for(int i = 0; i < N; i++) {temp_dic[i] = i;}
    int min_max_range = find_max(blocks, temp_dic);
    while (std::next_permutation(temp_dic.begin(), temp_dic.end())) {
        int max_range = find_max(blocks, temp_dic);
        if (max_range < min_max_range) {
            min_max_range = max_range;
            dictionary = temp_dic;
        }
    }
    std::printf("enum_alg:\n");
    print_perm(dictionary);
    std::printf("\t max_range = %d\n", min_max_range);
    std::printf("\t #bits after compression = %lf\n", compression(min_max_range));
    return min_max_range;
}

/* Random assignment */
int random_alg(std::vector<std::pair<int, int>>& blocks, std::vector<int>& dictionary) {
    for(int i = 0; i < N; i++) {
        dictionary[i] = i;
    }
    unsigned seed = std::time(0);
    std::shuffle(dictionary.begin(), dictionary.end(), std::default_random_engine(seed));
    std::printf("random_alg:\n");
    //print_perm(dictionary);
    int max_range = find_max(blocks, dictionary);
    std::printf("\t max_range = %d\n", max_range);
    std::printf("\t #bits after compression = %lf\n", compression(max_range));
    return max_range;
}

/* Algorithm 1 */
int find_min_degree(std::vector<int>& degrees, std::set<int>& vertices) {
    int min_degree = INT_MAX;
    int min_degree_vertex = N;
    for(auto& v : vertices) {
        if (min_degree > degrees[v]) {
            min_degree = degrees[v];
            min_degree_vertex = v;
        }
    }
    return min_degree_vertex;
}
int alg_1(std::vector<std::pair<int, int>>& blocks, std::vector<int>& dictionary) {
    std::vector<int> degrees(N);
    std::set<int> undiscovered_vertices;
    for(int i = 0; i < N; i++){degrees[i] = 0; undiscovered_vertices.insert(i);}
    std::vector<std::set<int>> neighbours(N);
    for(auto& b : blocks) {
        if (neighbours[b.first].count(b.second) == 0) {
            neighbours[b.first].insert(b.second);
            neighbours[b.second].insert(b.first);
            degrees[b.first]++;
            degrees[b.second]++;
        }
    }
    std::vector<int> order_by_degree;
    int idx = 0;
    while (undiscovered_vertices.size() > 0) {
        int current_vertex;
        if (idx == order_by_degree.size()) {
            current_vertex = find_min_degree(degrees, undiscovered_vertices);
            order_by_degree.push_back(current_vertex);
            undiscovered_vertices.erase(current_vertex);
        } else {
            current_vertex = order_by_degree[idx];
        }
        //std::printf("current_vertex = %d\n", current_vertex);/////////////////////////////////////////////////
        for(auto& v : neighbours[current_vertex]) {
            neighbours[v].erase(current_vertex);
            degrees[v]--;
        }
        while(neighbours[current_vertex].size() > 0) {
            int next_vertex = find_min_degree(degrees, neighbours[current_vertex]);
            //std::printf("\t next_vertex = %d\n", next_vertex);////////////////////////////////////////////////
            if (undiscovered_vertices.count(next_vertex) > 0){
                order_by_degree.push_back(next_vertex);
                undiscovered_vertices.erase(next_vertex);
            }
            neighbours[current_vertex].erase(next_vertex);
        }
        idx++;
    }
    assert(order_by_degree.size() == N);
    for(int i = 0; i < N; i++) {
        dictionary[order_by_degree[i]] = i;
    }
    std::printf("alg_1:\n");
    //print_perm(dictionary);
    int max_range = find_max(blocks, dictionary);
    std::printf("\t max_range = %d\n", max_range);
    std::printf("\t #bits after compression = %lf\n", compression(max_range));
    return max_range;
}

int main() {
    // Construct the table
    std::vector<std::pair<int, int>> blocks(M);
    std::srand(std::time(0));
    for(int i = 0; i < M; i++) {
        blocks[i] = std::make_pair(rand() % N, rand() % N);
        while (blocks[i].second == blocks[i].first) {
            blocks[i].second = rand() % N;
        }
    }
    //for(auto& a : blocks){std::printf("(%d, %d)\n", a.first, a.second);}
    std::vector<int> dictionary(N);
    random_alg(blocks, dictionary);
    //enum_alg(blocks, dictionary);
    alg_1(blocks, dictionary);
    return 0;
}