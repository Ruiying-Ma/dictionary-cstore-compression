#include <bits/stdc++.h>
int B = 6; // number of tuples per block
int N = 1000; // number of different symbols
int M = 2 * N / B; // number of different blocks

void print_perm(std::vector<int>& dictionary) {
    std::printf("\t The permutation is : ");
    for(auto& a : dictionary){std::printf("%d ", a);}
    std::printf("\n");
}

/* Compute the approx number of bits used after compression */
double compression(double max_range) {
    return (double)M * log(N) + (double)M * (double)(B - 1) * log(max_range);
}

/* Find the minimum and the maximum of a block */
std::pair<int, int> min_and_max_elem(std::multiset<int>& block, std::vector<int>& dictionary) {
    int min = INT_MAX;
    int max = INT_MIN;
    for(auto& a : block) {
        if (dictionary[a] < min) {min = dictionary[a];}
        if (dictionary[a] > max) {max = dictionary[a];}
    }
    return std::make_pair(min, max);
}

/* Find the maximal diameter of blocks under certain assignment */
int find_max(std::vector<std::multiset<int>>& blocks, std::vector<int>& dictionary) {
    int max_range = 0;
    for(auto& a : blocks) {
        std::pair<int, int> min_and_max = min_and_max_elem(a, dictionary);
        int cur_range = min_and_max.second - min_and_max.first;
        if (cur_range > max_range) {max_range = cur_range;}
    }
    return max_range;
}

/* Enumerate all possible assignments to find the optimal assignment */
int enum_alg(std::vector<std::multiset<int>>& blocks, std::vector<int>& dictionary) {
    std::vector<int> temp_dic(N);
    for(int i = 0; i < N; i++){temp_dic[i] = i;}
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
int random_alg(std::vector<std::multiset<int>>& blocks, std::vector<int>& dictionary) {
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

/* Find the symbol in `symbols` with the minimal frequency */
int find_min_freq(std::vector<int>& freq, std::set<int> symbols) {
    int min_freq = INT_MAX;
    int min_freq_symbol = N;
    for(auto& s : symbols) {
        if (freq[s] < min_freq) {
            min_freq = freq[s];
            min_freq_symbol = s;
        }
    }
    return min_freq_symbol;
}

/* Algorithm 2 */
int alg_2(std::vector<std::multiset<int>>& blocks, std::vector<int>& dictionary) {
    std::vector<int> freq(N);
    std::set<int> undiscovered_symbols;
    for(int i = 0; i < N; i++) {freq[i] = 0; undiscovered_symbols.insert(i);}
    std::vector<std::set<int>> neighbours(N);
    for(auto& b : blocks) {
        // Deduplicate
        std::set<int> uni_block;
        for(auto& s : b) {if (uni_block.count(s) == 0){uni_block.insert(s);}}
        for (auto& s : uni_block) {
            for(auto& ss : uni_block) {
                if (ss != s && neighbours[s].count(ss) == 0) {
                    neighbours[s].insert(ss);
                    freq[s]++;
                }
            }
        }
    }
    /*
    std::printf("neighbours:\n");
    for(int i = 0; i < N; i++) {
        std::printf("\t %d (freq = %d): ", i, freq[i]);
        for(auto& s : neighbours[i]){std::printf("%d ", s);}
        std::printf("\n");
    }
    */
    std::vector<int> order_by_freq;
    int idx = 0;
    while (undiscovered_symbols.size() > 0) {
        int current_symbol;
        if (order_by_freq.size() == idx) {
            current_symbol = find_min_freq(freq, undiscovered_symbols);
            order_by_freq.push_back(current_symbol);
            undiscovered_symbols.erase(current_symbol);
        } else {
            current_symbol = order_by_freq[idx];
        }
        //std::printf("current_symbol = %d\n", current_symbol);/////////////////////////////////////////////////
        for(auto& s : neighbours[current_symbol]) {
            neighbours[s].erase(current_symbol);
            freq[s]--;
        }
        while(neighbours[current_symbol].size() > 0) {
            int next_symbol = find_min_freq(freq, neighbours[current_symbol]);
            //std::printf("\t next_symbol = %d\n", next_symbol);////////////////////////////////////////////////
            if(undiscovered_symbols.count(next_symbol) > 0) {
                order_by_freq.push_back(next_symbol);
                undiscovered_symbols.erase(next_symbol);
            }
            neighbours[current_symbol].erase(next_symbol);
        }
        idx++;
    }
    assert(order_by_freq.size() == N);
    for(int i = 0; i < N; i++) {
        dictionary[order_by_freq[i]] = i;
    }
    std::printf("alg_2:\n");
    //print_perm(dictionary);
    int max_range = find_max(blocks, dictionary);
    std::printf("\t max_range = %d\n", max_range);
    std::printf("\t #bits after compression = %lf\n", compression(max_range));
    return max_range;
}

int main() {
    // Construct the table
    std::vector<std::multiset<int>> blocks(M);
    std::srand(std::time(0));
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < B - 1; j++) {blocks[i].insert(rand() % N);}
        // Ensure that the block contains at least 2 distinct symbols
        int t = rand() % N;
        while (blocks[i].count(t) == B - 1) {
            t = rand() % N;
        }
        blocks[i].insert(t);
    }
    //for(auto& a : blocks) {for(auto& b : a) {std::printf("%d ", b);} std::printf("\n");}
    std::vector<int> dictionary(N);
    random_alg(blocks, dictionary);
    //enum_alg(blocks, dictionary);
    alg_2(blocks, dictionary);
    return 0;
}