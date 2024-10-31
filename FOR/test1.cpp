#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;

struct DataAttr {
    int m_front_freq = 0;
    int m_dict = -1;
    int m_list = -1;
};

void CheckBuild(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column, std::map<std::pair<std::string, std::string>, int> &data_pairs_pool) {
    for(auto& block : column) {
        assert(data_pool.count(block.first) > 0);
        for(auto& data : block.second) {
            assert(data_pool.count(data) > 0);
            if (data != block.first) {
                std::string left = (block.first < data) ? block.first : data;
                std::string right = (block.first < data) ? data : block.first;
                assert(data_pairs_pool.count(std::make_pair(left, right)) > 0);
            }
        }
    }

    for(auto& pair : data_pairs_pool) {
        assert(data_pool.count(pair.first.first) > 0);
        assert(data_pool.count(pair.first.second) > 0);
    }
}

double ReadData(int data_set, std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column, std::map<std::pair<std::string, std::string>, int> &data_pairs_pool) {
    std::ifstream read_file;
    std::string file_name = "../Data/ml_data/data_";
    file_name += std::to_string(data_set);
    file_name += ".txt";
    read_file.open(file_name, std::ios::in);

    std::clock_t start = std::clock();

    int block = 0;
    int offset = 0;
    if (read_file.is_open()) {
        std::string data;
        while (std::getline(read_file, data)) {
            assert(block >= 0 && block < BLOCK_NUM && offset >= 0 && offset < BLOCK_SIZE);
            if (offset == 0) {
                // head data
                // column
                column[block].first = data;
                assert(column[block].second.empty());
                // data_pool
                if(data_pool.count(data) > 0) {
                    data_pool[data]->m_front_freq++;
                } else {
                    DataAttr* data_attr = new DataAttr;
                    data_attr->m_front_freq++;
                    data_pool.emplace(data, data_attr);
                }
            } else {
                // body data
                // column
                if (column[block].second.count(data) == 0) {
                    column[block].second.insert(data);
                    // data_pairs_pool
                    if (data != column[block].first) {
                        std::string left = (column[block].first < data) ? column[block].first : data;
                        std::string right = (column[block].first < data) ? data : column[block].first;
                        std::pair<std::string, std::string> data_pair = std::make_pair(left, right);
                        if (data_pairs_pool.count(data_pair) > 0) {
                            data_pairs_pool[data_pair]++;
                        } else {
                            data_pairs_pool.emplace(data_pair, 1);
                        }
                    }
                }
                // data_pool
                if (data_pool.count(data) == 0) {
                    DataAttr* data_attr = new DataAttr;
                    data_pool.emplace(data, data_attr);
                }
            }
            offset++;
            if (offset == BLOCK_SIZE) {
                assert(data_pool.count(column[block].first) > 0);
                assert(!column[block].second.empty());
                block++;
                offset = 0;
            }
        }
    } else {
        assert("Fail to open the file\n");
    }
    std::clock_t end = std::clock();
    CheckBuild(data_pool, column, data_pairs_pool);
    return (double) (end - start) / (double)CLOCKS_PER_SEC;
}

void CheckAssign(std::map<std::string, DataAttr*> &data_pool) {
    std::vector<bool> dictionary;
    dictionary.resize(data_pool.size());
    for(int i = 0; i < dictionary.size(); i++) {
        dictionary[i] = false;
    }
    for(auto& data : data_pool) {
        int dict = data.second->m_dict;
        assert(dict >=1 && dict <= data_pool.size() && (dictionary[dict-1] == false));
        dictionary[dict-1] = true;
    }
}

int Compute(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    CheckAssign(data_pool);
    int cost = 0;
    for(auto& block : column) {
        int block_max = INT_MIN;
        assert(data_pool.count(block.first) > 0);
        int front_dict = data_pool[block.first]->m_dict;
        assert(front_dict >= 1 && front_dict <= data_pool.size() && (data_pool[block.first]->m_list >= 0));
        for(auto& data : block.second) {
            assert(data_pool.count(data) > 0);
            int dict = data_pool[data]->m_dict;
            assert(dict >= 1 && dict <= data_pool.size() && (data_pool[data]->m_list >= 0));
            if (std::abs(dict - front_dict) > block_max) {
                block_max = std::abs(dict - front_dict);
            }
        }
        assert(block_max >= 0);
        if (block_max == 0){block_max = 1;}
        cost += (int)floor(log2((double)block_max) + (double)1) * (BLOCK_SIZE - 1) + (int)floor(log2((double)front_dict) + (double)1);
    }
    assert(cost >= 2 * BLOCK_NUM * BLOCK_SIZE);
    return cost;
}

// Append the second list to the first list's back
// Second list is appended in order
void append_back_in_order(std::map<std::string, DataAttr*> &data_pool, std::map<int, std::list<std::string>> &data_lists, DataAttr* first_data, DataAttr* second_data) {
    int second_list_idx = second_data->m_list;
    for(auto iter = data_lists[second_list_idx].begin(); iter != data_lists[second_list_idx].end(); ++iter) {
        assert(data_pool.count(*iter) > 0);
        data_pool[*iter]->m_list = first_data->m_list;
        data_lists[first_data->m_list].push_back(*iter);
    }
    data_lists.erase(second_list_idx);
    assert(data_lists.size() > 0);
}

// Append the second list to the first list's back
// Second list is appended in reverse order
void append_back_reverse_order(std::map<std::string, DataAttr*> &data_pool, std::map<int, std::list<std::string>> &data_lists, DataAttr* first_data, DataAttr* second_data) {
    int second_list_idx = second_data->m_list;
    for(auto iter = data_lists[second_list_idx].rbegin(); iter != data_lists[second_list_idx].rend(); iter++) {
        assert(data_pool.count(*iter) > 0);
        data_pool[*iter]->m_list = first_data->m_list;
        data_lists[first_data->m_list].push_back(*iter);
    }
    data_lists.erase(second_list_idx);
    assert(data_lists.size() > 0);
}

// Append the second list to the first list's front
// Second list is appended in order
void append_front_in_order(std::map<std::string, DataAttr*> &data_pool, std::map<int, std::list<std::string>> &data_lists, DataAttr* first_data, DataAttr* second_data) {
    int second_list_idx = second_data->m_list;
    for(auto iter = data_lists[second_list_idx].rbegin(); iter != data_lists[second_list_idx].rend(); iter++) {
        assert(data_pool.count(*iter) > 0);
        data_pool[*iter]->m_list = first_data->m_list;
        data_lists[first_data->m_list].push_front(*iter);
    }
    data_lists.erase(second_list_idx);
    assert(data_lists.size() > 0);
}

// Append the second list to the first list's front
// Second list is appended in reverse order
void append_front_reverse_order(std::map<std::string, DataAttr*> &data_pool, std::map<int, std::list<std::string>> &data_lists, DataAttr* first_data, DataAttr* second_data) {
    int second_list_idx = second_data->m_list;
    for(auto iter = data_lists[second_list_idx].begin(); iter != data_lists[second_list_idx].end(); iter++) {
        assert(data_pool.count(*iter) > 0);
        data_pool[*iter]->m_list = first_data->m_list;
        data_lists[first_data->m_list].push_front(*iter);
    }
    data_lists.erase(second_list_idx);
    assert(data_lists.size() > 0);
}

int AssignByPairFreq(std::map<std::string, DataAttr*> &data_pool, std::map<std::pair<std::string, std::string>, int> &data_pairs_pool, std::map<int, std::list<std::string>> &data_lists, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    if (data_pairs_pool.size() == 0) {
        assert(data_pool.size() == 1);
        for(auto& data : data_pool) {
            data.second->m_dict = 1;
            data.second->m_list = 0;
        }
        return Compute(data_pool, column);
    }
    std::vector<std::pair<std::pair<std::string, std::string>, int>> data_pairs_vec;
    std::copy(data_pairs_pool.begin(), data_pairs_pool.end(), std::back_inserter<std::vector<std::pair<std::pair<std::string, std::string>, int>>>(data_pairs_vec));
    assert(data_pairs_vec.size() == data_pairs_pool.size());
    std::sort(data_pairs_vec.begin(), data_pairs_vec.end(), [](const std::pair<std::pair<std::string, std::string>, int> &data_pair1, const std::pair<std::pair<std::string, std::string>, int> &data_pair2) {
        if (data_pair1.second != data_pair2.second) {
            return data_pair1.second > data_pair2.second;
        } else if (data_pair1.first.first != data_pair2.first.first) {
            return data_pair1.first.first < data_pair2.first.first;
        } 
        assert(data_pair1.first.second != data_pair2.first.second);
        return data_pair1.first.second < data_pair2.first.second;
    });
    int list_cnt = 0;
    for(auto& data_pair : data_pairs_vec) {
        assert(data_pool.count(data_pair.first.first) > 0);
        assert(data_pool.count(data_pair.first.second) > 0);
        DataAttr* first_data = data_pool[data_pair.first.first];
        DataAttr* second_data = data_pool[data_pair.first.second];
        if ((first_data->m_list == -1) && (second_data->m_list == -1)) {
            assert(data_lists.count(list_cnt) == 0);
            data_lists.emplace(list_cnt, std::list<std::string>{data_pair.first.first, data_pair.first.second});
            first_data->m_list = list_cnt;
            second_data->m_list = list_cnt;
            list_cnt++;
        } else if ((first_data->m_list == -1) && (second_data->m_list >= 0)) {
            assert(data_lists.count(second_data->m_list) > 0);
            std::list<std::string>::iterator iter = data_lists[second_data->m_list].begin();
            int left = 0, right = 0;
            while ((*iter) != data_pair.first.second) {
                assert(iter != data_lists[second_data->m_list].end());
                left++;
                iter++;
            }
            iter++;
            while (iter != data_lists[second_data->m_list].end()) {
                assert((*iter) != data_pair.first.second);
                right++;
                iter++;
            }
            if (left <= right) {
                data_lists[second_data->m_list].push_front(data_pair.first.first);
            } else {
                data_lists[second_data->m_list].push_back(data_pair.first.first);
            }
            first_data->m_list = second_data->m_list;
        } else if ((first_data->m_list >= 0) && (second_data->m_list == -1)) {
            assert(data_lists.count(first_data->m_list) > 0);
            std::list<std::string>::iterator iter = data_lists[first_data->m_list].begin();
            int left = 0, right = 0;
            while ((*iter) != data_pair.first.first) {
                assert(iter != data_lists[first_data->m_list].end());
                left++;
                iter++;
            }
            iter++;
            while (iter != data_lists[first_data->m_list].end()) {
                assert((*iter) != data_pair.first.first);
                right++;
                iter++;
            }
            if (left <= right) {
                data_lists[first_data->m_list].push_front(data_pair.first.second);
            } else {
                data_lists[first_data->m_list].push_back(data_pair.first.second);
            }
            second_data->m_list = first_data->m_list;
        } else if ((first_data->m_list >= 0) && (second_data->m_list >= 0) && (first_data->m_list != second_data->m_list)) {
            assert(data_lists.count(first_data->m_list) > 0 && data_lists.count(second_data->m_list) > 0 );
            bool first_list_shorter = (data_lists[first_data->m_list].size() <= data_lists[second_data->m_list].size());
            int first_list_left = 0, first_list_right = 0;
            std::list<std::string>::iterator first_iter = data_lists[first_data->m_list].begin();
            while ((*first_iter) != data_pair.first.first) {
                assert(first_iter != data_lists[first_data->m_list].end());
                first_list_left++;
                first_iter++;
            }
            first_iter++;
            while (first_iter != data_lists[first_data->m_list].end()) {
                assert((*first_iter) != data_pair.first.first);
                first_list_right++;
                first_iter++;
            }

            int second_list_left = 0, second_list_right = 0;
            std::list<std::string>::iterator second_iter = data_lists[second_data->m_list].begin();
            while ((*second_iter) != data_pair.first.second) {
                assert(second_iter != data_lists[second_data->m_list].end());
                second_list_left++;
                second_iter++;
            }
            second_iter++;
            while (second_iter != data_lists[second_data->m_list].end()) {
                assert((*second_iter) != data_pair.first.second);
                second_list_right++;
                second_iter++;
            }
            
            // combine two lists
            int min_distance = INT_MAX;
            min_distance = ((first_list_left + second_list_left) < min_distance) ? first_list_left + second_list_left : min_distance;
            min_distance = ((first_list_left + second_list_right) < min_distance) ? first_list_left + second_list_right : min_distance;
            min_distance = ((first_list_right + second_list_left) < min_distance) ? first_list_right + second_list_left : min_distance;
            min_distance = ((first_list_right + second_list_right) < min_distance) ? first_list_right + second_list_right : min_distance;
            if (min_distance == first_list_left + second_list_right) {
                // L2R2 + L1R1
                if (first_list_shorter) {
                    // append list1 to list2's back
                    append_back_in_order(data_pool, data_lists, second_data, first_data);
                } else {
                    // append list2 to list1's front
                    append_front_in_order(data_pool, data_lists, first_data, second_data);
                }
            } else if (min_distance == first_list_right + second_list_left) {
                // L1R1 + L2R2
                if (first_list_shorter) {
                    // append list1 to list2's front
                    append_front_in_order(data_pool, data_lists, second_data, first_data);
                } else {
                    // append list2 to list1's back
                    append_back_in_order(data_pool, data_lists, first_data, second_data);
                }
            } else if (min_distance == first_list_left + second_list_left) {
                if (first_list_shorter) {
                    // R1L1 + L2R2
                    // append list1 to list2's front
                    append_front_reverse_order(data_pool, data_lists, second_data, first_data);
                } else {
                    // R2L2 + L1R1
                    // append list2 to list1's front
                    append_front_reverse_order(data_pool, data_lists, first_data, second_data);
                }
            } else {
                assert(min_distance == first_list_right + second_list_right);
                if (first_list_shorter) {
                    // L2R2 + R1L1
                    // append list1 to list2's back
                    append_back_reverse_order(data_pool, data_lists, second_data, first_data);
                } else {
                    // L1R1 + R2L2
                    // append list2 to list1's back
                    append_back_reverse_order(data_pool, data_lists, first_data, second_data);
                }
            }

        } else {
            assert(first_data->m_list == second_data->m_list);
        }
        assert(first_data->m_list >= 0 && second_data->m_list >=0);
    }

    assert(data_lists.size() > 0);
    std::vector<std::pair<DataAttr*, bool>> list_max_front_freq;
    for(auto& list : data_lists) {
        int idx = 0;
        int max_front_freq = INT_MIN;
        int max_front_freq_idx = -1;
        std::string max_front_freq_data;
        for(auto iter = list.second.begin(); iter != list.second.end(); iter++){
            assert(data_pool.count(*iter) > 0);
            if (max_front_freq < data_pool[*iter]->m_front_freq) {
                max_front_freq = data_pool[*iter]->m_front_freq;
                max_front_freq_idx = idx;
                max_front_freq_data = *iter;
            }
            idx++;
        }
        assert(idx == list.second.size());
        if (max_front_freq_idx <= list.second.size() - 1 - max_front_freq_idx) {
            list_max_front_freq.emplace_back(data_pool[max_front_freq_data], false);
        } else {
            list_max_front_freq.emplace_back(data_pool[max_front_freq_data], true);
        }
    }
    assert(list_max_front_freq.size() == data_lists.size());
    std::sort(list_max_front_freq.begin(), list_max_front_freq.end(), [](const std::pair<DataAttr*, bool> &list1, const std::pair<DataAttr*, bool> &list2) {
        if (list1.first->m_front_freq != list2.first->m_front_freq) {
            return list1.first->m_front_freq > list2.first->m_front_freq;
        }
        assert(list1.first != list2.first);
        return list1.first < list2.first;
    });
    int dict = 1;
    for(auto& max_front : list_max_front_freq) {
        if (max_front.second) {
            // need to reverse the assigning order
            for (auto iter = data_lists[max_front.first->m_list].rbegin(); iter != data_lists[max_front.first->m_list].rend(); iter++) {
                assert(data_pool.count(*iter) > 0 && data_pool[*iter]->m_dict == -1);
                data_pool[*iter]->m_dict = dict;
                dict++;
            }
        } else {
            for (auto iter = data_lists[max_front.first->m_list].begin(); iter != data_lists[max_front.first->m_list].end(); iter++) {
                assert(data_pool.count(*iter) > 0 && data_pool[*iter]->m_dict == -1);
                data_pool[*iter]->m_dict = dict;
                dict++;
            }
        }
    }
    assert(dict == (data_pool.size() + 1));
    return Compute(data_pool, column);
}

int AssignByRandom(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    std::vector<std::pair<std::string, DataAttr*>> data_vec;
    std::copy(data_pool.begin(), data_pool.end(), std::back_inserter<std::vector<std::pair<std::string, DataAttr*>>>(data_vec));
    assert(data_vec.size() == data_pool.size());
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(data_vec.begin(), data_vec.end(), std::default_random_engine(seed));
    for(int i = 0; i < data_vec.size(); i++) {
        data_vec[i].second->m_dict = i + 1  ;
    }
    return Compute(data_pool, column);
}

int AssignByFrontFreq(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    std::vector<std::pair<std::string, DataAttr*>> data_vec;
    std::copy(data_pool.begin(), data_pool.end(), std::back_inserter<std::vector<std::pair<std::string, DataAttr*>>>(data_vec));
    assert(data_vec.size() == data_pool.size());
    std::sort(data_vec.begin(), data_vec.end(), [](const std::pair<std::string, DataAttr*> &data1, const std::pair<std::string, DataAttr*> &data2){
        if (data1.second->m_front_freq != data2.second->m_front_freq) {return data1.second->m_front_freq < data2.second->m_front_freq;}
        return data1.first < data2.first;
    });
    for(int i = 0; i < data_vec.size(); i++) {
        data_vec[i].second->m_dict = data_vec.size() - i;
    }
    return Compute(data_pool, column);
}

int main(int argc, char *argv[]) {
    int data_set = std::atoi(argv[1]);
    std::printf("data_%d.txt\t", data_set);
    std::vector<std::pair<std::string, std::set<std::string>>> column;
    column.resize(BLOCK_NUM);
    std::map<std::string, DataAttr*> data_pool;
    std::map<int, std::list<std::string>> data_list;
    std::map<std::pair<std::string, std::string>, int> data_pairs_pool;
    // Read data
    double read_data_time = ReadData(data_set, data_pool, column, data_pairs_pool);
    // Compute
    std::clock_t start = std::clock();
    double result_by_pair_freq = (double) AssignByPairFreq(data_pool, data_pairs_pool, data_list, column);
    std::clock_t end = std::clock();
    double compute_pair_freq_time = (double)(end - start) / (double) CLOCKS_PER_SEC;

    double result_by_front_freq = (double) AssignByFrontFreq(data_pool, column);

    double ratio_pair_rand = 0;
    double ratio_front_rand = 0;
    for(int i = 0; i < 10; i++) {
        double result_by_rand = (double) AssignByRandom(data_pool, column);
        ratio_pair_rand += result_by_pair_freq / result_by_rand;
        ratio_front_rand += result_by_front_freq / result_by_rand;
    }
    ratio_pair_rand /= 10;
    ratio_front_rand /= 10;

    double ndv = (double) data_pool.size() * (double)100 / (double) BLOCK_NUM / (double) BLOCK_SIZE;
    double total_pair_freq_time = read_data_time + compute_pair_freq_time;
    
    // Result
    std::printf("ndv = %.6lf\t", ndv);
    std::printf("Pair/Rand = %.6lf\t", ratio_pair_rand);
    std::printf("Front/Rand = %.6lf\t", ratio_front_rand);
    std::printf("read_data_time = %.6lf\t", read_data_time);
    std::printf("compute_pair_freq_time = %.6lf\t", compute_pair_freq_time);
    std::printf("total_pair_freq_time = %.6lf\n", total_pair_freq_time);

    return 0;
    
}