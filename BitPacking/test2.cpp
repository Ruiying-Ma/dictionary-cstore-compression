#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;

struct DataAttr{
    int m_freq = 1;
    int m_dict = -1;
    std::string m_data;
    std::set<int> m_blocks;
};

struct DataComp
{
    bool operator()(const DataAttr* lhs, const DataAttr* rhs) const
    {
        if (lhs->m_freq != rhs->m_freq) {return lhs->m_freq < rhs->m_freq;}
        return lhs->m_data < rhs->m_data;
    }
};
 
typedef std::set<DataAttr*, DataComp> data_pq;


void CheckBuild(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
    for(auto& data : data_pool) {
        assert(data.second->m_freq == data.second->m_blocks.size());
    }
    for(int block = 0; block < BLOCK_NUM; block++) {
        for(auto& data : column[block]) {
            assert(data_pool[data]->m_blocks.count(block) > 0);
        }
    }
}

void BuildDataPQ(std::map<std::string, DataAttr*> &data_pool, data_pq &data_heap) {
    for(auto& data : data_pool) {
        assert(data_heap.count(data.second) == 0);
        data_heap.insert(data.second);
    }
    assert(data_heap.size() == data_pool.size());
    return;
}

double BuildData(int data_set, std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column, data_pq &data_heap) {
    std::ifstream read_file;
    std::string file_name = "data_";
    file_name += std::to_string(data_set);
    file_name += ".txt";
    read_file.open(file_name, std::ios::in);

    std::clock_t start = std::clock();
    int block = 0;
    int offset = 0;
    if (read_file.is_open()) {
        std::string data;
        while (std::getline(read_file, data)) {
            assert(block >=0 && block < BLOCK_NUM && offset >=0 && offset < BLOCK_SIZE);
            assert(column[block].size() >= 0 && column[block].size() <= BLOCK_SIZE);
            if (offset == 0) {assert(column[block].size() == 0);}
            if (column[block].count(data) > 0) {
                assert(data_pool.count(data) > 0);
            } else {
                column[block].insert(data);
                if (data_pool.count(data) > 0) {
                    data_pool[data]->m_freq++;
                    data_pool[data]->m_blocks.insert(block);
                } else {
                    DataAttr* data_attr = new DataAttr;
                    data_attr->m_data = data;
                    data_attr->m_blocks.insert(block);
                    data_pool.emplace(data, data_attr);
                }
            }
            offset++;
            if (offset == BLOCK_SIZE) {
                assert(column[block].size() > 0);
                offset = 0;
                block++;
            }
        } 
    } else {
        assert("Fail to open the file\n");
    }

    BuildDataPQ(data_pool, data_heap);

    std::clock_t end = std::clock();
    read_file.close();
    CheckBuild(data_pool, column);
    return (double) (end - start) / (double) CLOCKS_PER_SEC;
    // //////////
    // for(auto& data : data_pool) {
    //     std::cout<<"data = "<<data.first<<", freq = "<<data.second->m_freq<<"\n";
    // }
    // std::cout<<"ndv = "<<(double)data_pool.size() * (double)100 / (double)BLOCK_NUM / (double)BLOCK_SIZE<<"%\n";
    // std::cout<<"Finish Reaing\n";
    // ////////

}

// double BuildDataPairs(std::vector<std::set<std::string>> &column, std::map<std::pair<std::string, std::string>, int> &data_pairs, std::map<std::string, DataAttr*> &data_pool) {
//     std::clock_t start = std::clock();
//     for(auto& block : column) {
//         for(auto& data : block) {
//             for(auto& another_data : block) {
//                 if (another_data != data) {
//                     std::string left = (data < another_data) ? data : another_data;
//                     std::string right = (data < another_data) ? another_data : data;
//                     std::pair<std::string, std::string> pair = std::make_pair(left, right);
//                     if (data_pairs.count(pair) > 0) {
//                         data_pairs[pair]++;
//                     } else {
//                         data_pairs.emplace(pair, 1);
//                     }
//                 }
//             }
//         }
//     }
//     std::clock_t end = std::clock();
//     CheckBuild(data_pool, data_pairs);
//     return (double)(end - start) / (double)CLOCKS_PER_SEC;
// }

void update_data_pool(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column, DataAttr* assigned_data_attr) {
    for(auto& block : assigned_data_attr->m_blocks) {
        for(auto& data : column[block]) {
            if (data != assigned_data_attr->m_data && data_pool[data]->m_dict == -1) {
                assert(data_pool[data]->m_blocks.count(block) > 0);
                assert(data_pool[data]->m_freq > 0);
                data_pool[data]->m_freq--;
                data_pool[data]->m_blocks.erase(block);
                assert(data_pool[data]->m_freq >=0);
                assert(data_pool[data]->m_blocks.size() == data_pool[data]->m_freq);
            } else if (data != assigned_data_attr->m_data) {
                assert(data_pool[data]->m_freq == 0);
                assert(data_pool[data]->m_blocks.empty());
            }
        }
    }
    assigned_data_attr->m_blocks.clear();
    assigned_data_attr->m_freq = 0;
}

int Compute(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
    int cost = 0;
    for(auto& block : column) {
        int max_dict_value = -1;
        for(auto& data : block) {
            assert(data_pool.count(data) > 0);
            if (data_pool[data]->m_dict > max_dict_value) {
                max_dict_value = data_pool[data]->m_dict;
            }
        }
        assert(max_dict_value > 0);
        cost += (int)floor(log2(max_dict_value) + 1);
    }
    return cost;
}

int AssignByFreq(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column, data_pq &data_heap) {
    int dict = data_pool.size();
    while (dict > 0) {
        assert(dict == data_heap.size());
        auto min_freq_data_iter = data_heap.begin();
        DataAttr* min_freq_data_attr = *min_freq_data_iter;
        update_data_pool(data_pool, column, min_freq_data_attr);
        min_freq_data_attr->m_dict = dict;
        data_heap.erase(min_freq_data_iter);
        dict--;
    }
    assert(data_heap.size() == 0);
    // Check
    for(auto& data : data_pool) {
        assert(data.second->m_freq == 0 && data.second->m_dict > 0);
    }

    return Compute(data_pool, column);
}

int AssignByRandom(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
    std::vector<std::pair<std::string, DataAttr*>> data_vec;
    std::copy(data_pool.begin(), data_pool.end(), std::back_inserter<std::vector<std::pair<std::string, DataAttr*>>>(data_vec));
    assert(data_vec.size() == data_pool.size());
    unsigned seed = std::time(0);
    std::shuffle(data_vec.begin(), data_vec.end(), std::default_random_engine(seed));
    for(int i = 0; i < data_vec.size(); i++) {
        data_vec[i].second->m_dict = i + 1;
    }
    return Compute(data_pool, column);
}


void CheckAssign(std::map<std::string, DataAttr*> &data_pool) {
    std::vector<bool> dict;
    dict.resize(data_pool.size());
    for(int i = 0; i < dict.size(); i++) {dict[i] = false;}
    for(auto& data : data_pool) {
        assert(data.second->m_dict > 0 && data.second->m_dict <= data_pool.size());
        assert(dict[data.second->m_dict - 1] == false);
        dict[data.second->m_dict - 1] = true;
    }
    for(int i = 0; i < dict.size(); i++) {assert(dict[i]);}
}

int main (int argc, char *argv[]) {
    int data_set = std::atoi(argv[1]);
    std::printf("%d\t", data_set);
    std::map<std::string, DataAttr*> data_pool;
    std::vector<std::set<std::string>> column;
    column.resize(BLOCK_NUM);
    data_pq data_heap;
    // Build Data
    double build_data_time = BuildData(data_set, data_pool, column, data_heap);
    double ndv = (double) data_pool.size() * (double) 100 / (double) BLOCK_NUM / (double) BLOCK_SIZE;
    // Compute
    std::clock_t start = std::clock();
    double result_by_freq = (double) AssignByFreq(data_pool, column, data_heap);
    std::clock_t end = std::clock();
    CheckAssign(data_pool);
    double result_by_rand = (double) AssignByRandom(data_pool, column);
    double ratio = result_by_freq / result_by_rand;
    double compute_time = (double) (end-start) / (double) CLOCKS_PER_SEC;
    double total_time = build_data_time + compute_time;
    // Result
    std::printf("%.6lf\t", ndv);
    std::printf(" %.6lf\t", ratio);
    std::printf("%.6lf\t", build_data_time);
    std::printf("%.6lf\t", compute_time);
    std::printf("%.6lf\n", total_time);
    // Release Meomry
    for(auto& data : data_pool) {
        delete data.second;
    }
}