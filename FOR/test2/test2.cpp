# include "DataAttr.hpp"
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;

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
    std::string file_name = "../../Data/ml_data/data_" + std::to_string(data_set) + ".txt";
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
                    data_pool[data]->increment_front_freq();
                } else {
                    DataAttr* data_attr = new DataAttr(data);
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
                    DataAttr* data_attr = new DataAttr(data);
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
        int dict = data.second->get_dict();
        assert(dict >= 1 && dict <= data_pool.size() && (dictionary[dict-1] == false));
        dictionary[dict-1] = true;
    }
}

int Compute(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    CheckAssign(data_pool);
    int cost = 0;
    for(auto& block : column) {
        int block_max = INT_MIN;
        assert(data_pool.count(block.first) > 0);
        int front_dict = data_pool[block.first]->get_dict();
        assert(front_dict >= 1 && front_dict <= data_pool.size());
        for(auto& data : block.second) {
            assert(data_pool.count(data) > 0);
            int dict = data_pool[data]->get_dict();
            assert(dict >= 1 && dict <= data_pool.size());
            if (std::abs(dict - front_dict) > block_max) {
                block_max = std::abs(dict - front_dict);
            }
        }
        assert(block_max >= 0);
        if (block_max == 0){block_max = 1;}
        cost += (int)floor(log2((double)block_max) + (double)2) * (BLOCK_SIZE - 1) + (int)floor(log2((double)front_dict) + (double)2); // consider sign bits
    }
    assert(cost >= 2 * BLOCK_NUM * BLOCK_SIZE);
    return cost;
}

int AssignByPairFreq(std::map<std::string, DataAttr*> &data_pool, std::map<std::pair<std::string, std::string>, int> &data_pairs_pool, std::map<int, OuterList> &outer_lists, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    if (data_pairs_pool.size() == 0) {
        assert(data_pool.size() == 1);
        for(auto& data : data_pool) {
            data.second->set_dict(1);
        }
        return Compute(data_pool, column);
    }
    int INNER_LIST_CAPACITY = (int)std::sqrt(data_pool.size());
    if (INNER_LIST_CAPACITY < 2) {INNER_LIST_CAPACITY = 2;}
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
    int outer_lists_cnt = 0;
    for(auto& data_pair : data_pairs_vec) {
        std::string first_data = data_pair.first.first;
        std::string second_data = data_pair.first.second;
        assert(data_pool.count(first_data) > 0);
        assert(data_pool.count(second_data) > 0);
        DataAttr* first_data_attr = data_pool[first_data];
        DataAttr* second_data_attr = data_pool[second_data];
        if ((first_data_attr->get_inner_list() == nullptr) && (second_data_attr->get_inner_list() == nullptr)) {
            assert(outer_lists.count(outer_lists_cnt) == 0);
            outer_lists.emplace(outer_lists_cnt, OuterList());
            outer_lists[outer_lists_cnt].set_id(outer_lists_cnt);
            first_data_attr->push_back(outer_lists[outer_lists_cnt], INNER_LIST_CAPACITY);
            second_data_attr->push_back(outer_lists[outer_lists_cnt], INNER_LIST_CAPACITY);
            outer_lists_cnt++;
        } else if ((first_data_attr->get_inner_list() != nullptr) && (second_data_attr->get_inner_list() == nullptr)) {
            int outer_list_id = first_data_attr->get_ourter_list_id();
            assert(outer_lists.count(outer_list_id) > 0);
            std::pair<int, int> left_right = first_data_attr->left_right(outer_lists[outer_list_id]);
            int left = left_right.first, right = left_right.second;
            if (left <= right) {second_data_attr->push_front(outer_lists[outer_list_id], INNER_LIST_CAPACITY);}
            else {second_data_attr->push_back(outer_lists[outer_list_id], INNER_LIST_CAPACITY);}
        } else if ((first_data_attr->get_inner_list() == nullptr) && (second_data_attr->get_inner_list() != nullptr)) {
            int outer_list_id = second_data_attr->get_ourter_list_id();
            assert(outer_lists.count(outer_list_id) > 0);
            std::pair<int, int> left_right = second_data_attr->left_right(outer_lists[outer_list_id]);
            int left = left_right.first, right = left_right.second;
            if (left <= right) {first_data_attr->push_front(outer_lists[outer_list_id], INNER_LIST_CAPACITY);}
            else {first_data_attr->push_back(outer_lists[outer_list_id], INNER_LIST_CAPACITY);}
        } else if ((first_data_attr->get_inner_list() != nullptr) && (second_data_attr->get_inner_list() != nullptr) && (first_data_attr->get_ourter_list_id() != second_data_attr->get_ourter_list_id())) {
            int first_outer_list_id = first_data_attr->get_ourter_list_id();
            int second_outer_list_id = second_data_attr->get_ourter_list_id();
            assert(outer_lists.count(first_outer_list_id) > 0 && outer_lists.count(second_outer_list_id) > 0);
            std::pair<int, int> first_left_right = first_data_attr->left_right(outer_lists[first_outer_list_id]);
            std::pair<int, int> second_left_right = second_data_attr->left_right(outer_lists[second_outer_list_id]);
            int first_left = first_left_right.first, first_right = first_left_right.second;
            int second_left = second_left_right.first, second_right = second_left_right.second;
            bool first_outer_list_shorter = (outer_lists[first_outer_list_id].m_list.size() <= outer_lists[second_outer_list_id].m_list.size());
            // combine two lists
            int min_distance = INT_MAX;
            min_distance = ((first_left + second_left) < min_distance) ? first_left + second_left : min_distance;
            min_distance = ((first_left + second_right) < min_distance) ? first_left + second_right : min_distance;
            min_distance = ((first_right + second_left) < min_distance) ? first_right + second_left : min_distance;
            min_distance = ((first_right + second_right) < min_distance) ? first_right + second_right : min_distance;
            if (min_distance == first_left + second_right) {
                // L2R2 + L1R1
                if (first_outer_list_shorter) {
                    // append list1 to list2's back
                    outer_lists[second_outer_list_id].push_back_in_order(outer_lists[first_outer_list_id]);
                    outer_lists.erase(first_outer_list_id);
                } else {
                    // append list2 to list1's front
                    outer_lists[first_outer_list_id].push_front_in_order(outer_lists[second_outer_list_id]);
                    outer_lists.erase(second_outer_list_id);
                }
            } else if (min_distance == first_right + second_left) {
                // L1R1 + L2R2
                if (first_outer_list_shorter) {
                    // append list1 to list2's front
                    outer_lists[second_outer_list_id].push_front_in_order(outer_lists[first_outer_list_id]);
                    outer_lists.erase(first_outer_list_id);
                } else {
                    // append list2 to list1's back
                    outer_lists[first_outer_list_id].push_back_in_order(outer_lists[second_outer_list_id]);
                    outer_lists.erase(second_outer_list_id);
                }
            } else if (min_distance == first_left + second_left) {
                if (first_outer_list_shorter) {
                    // R1L1 + L2R2
                    // append list1 to list2's front
                    outer_lists[second_outer_list_id].push_front_reverse_order(outer_lists[first_outer_list_id]);
                    outer_lists.erase(first_outer_list_id);
                } else {
                    // R2L2 + L1R1
                    // append list2 to list1's front
                    outer_lists[first_outer_list_id].push_front_reverse_order(outer_lists[second_outer_list_id]);
                    outer_lists.erase(second_outer_list_id);
                }
            } else {
                assert(min_distance == first_right + second_right);
                if (first_outer_list_shorter) {
                    // L2R2 + R1L1
                    // append list1 to list2's back
                    outer_lists[second_outer_list_id].push_back_reverse_order(outer_lists[first_outer_list_id]);
                    outer_lists.erase(first_outer_list_id);
                } else {
                    // L1R1 + R2L2
                    // append list2 to list1's back
                    outer_lists[first_outer_list_id].push_back_reverse_order(outer_lists[second_outer_list_id]);
                    outer_lists.erase(second_outer_list_id);
                }
            }

        } else {
            assert((first_data_attr->get_inner_list() != nullptr) && (second_data_attr->get_inner_list() != nullptr) && (first_data_attr->get_ourter_list_id() == second_data_attr->get_ourter_list_id()));
        }
        assert(first_data_attr->get_inner_list() != nullptr && second_data_attr->get_inner_list() != nullptr);
        assert(first_data_attr->get_ourter_list_id() >= 0 && second_data_attr->get_ourter_list_id() >= 0);
    }

    // Assign dict value
    assert(outer_lists.size() > 0);
    std::vector<std::pair<DataAttr*, bool>> outer_list_max_front_freq;
    for(auto& outer_list : outer_lists) {
        int idx = 0;
        assert(outer_list.first == outer_list.second.get_id());
        int max_front_freq = INT_MIN;
        int max_front_freq_idx = -1;
        std::string max_front_freq_data;
        for(auto outer_iter = outer_list.second.m_list.begin(); outer_iter != outer_list.second.m_list.end(); outer_iter++) {
            InnerList* inner_list = *outer_iter;
            for(auto inner_iter = inner_list->m_list.begin(); inner_iter != inner_list->m_list.end(); inner_iter++) {
                assert(data_pool.count(*inner_iter) > 0);
                if (max_front_freq < data_pool[*inner_iter]->get_front_freq()) {
                    max_front_freq = data_pool[*inner_iter]->get_front_freq();
                    max_front_freq_idx = idx;
                    max_front_freq_data = *inner_iter;
                }
                idx++;
            }
        }
        assert(idx == outer_list.second.get_size());
        if (max_front_freq_idx <= outer_list.second.get_size() - 1 - max_front_freq_idx) {
            outer_list_max_front_freq.emplace_back(data_pool[max_front_freq_data], false);
        } else {
            outer_list_max_front_freq.emplace_back(data_pool[max_front_freq_data], true);
        }
    }
    assert(outer_list_max_front_freq.size() == outer_lists.size());
    std::sort(outer_list_max_front_freq.begin(), outer_list_max_front_freq.end(), [](const std::pair<DataAttr*, bool> &list1, const std::pair<DataAttr*, bool> &list2) {
        if (list1.first->get_front_freq() != list2.first->get_front_freq()) {
            return list1.first->get_front_freq() > list2.first->get_front_freq();
        }
        assert(list1.first->get_data() != list2.first->get_data());
        return list1.first->get_data() < list2.first->get_data();
    });
    int dict = 1;
    for(auto& max_front : outer_list_max_front_freq) {
        if (max_front.second) {
            // need to reverse the assigning order
            int outer_list_id = max_front.first->get_ourter_list_id();
            for(auto outer_iter = outer_lists[outer_list_id].m_list.rbegin(); outer_iter != outer_lists[outer_list_id].m_list.rend(); outer_iter++) {
                InnerList* inner_list = *outer_iter;
                for(auto inner_iter = inner_list->m_list.rbegin(); inner_iter != inner_list->m_list.rend(); inner_iter++) {
                    assert(data_pool.count(*inner_iter) > 0);
                    data_pool[*inner_iter]->set_dict(dict);
                    dict++;
                }
            }
        } else {
            int outer_list_id = max_front.first->get_ourter_list_id();
            for(auto outer_iter = outer_lists[outer_list_id].m_list.begin(); outer_iter != outer_lists[outer_list_id].m_list.end(); outer_iter++) {
                InnerList* inner_list = *outer_iter;
                for(auto inner_iter = inner_list->m_list.begin(); inner_iter != inner_list->m_list.end(); inner_iter++) {
                    assert(data_pool.count(*inner_iter) > 0);
                    data_pool[*inner_iter]->set_dict(dict);
                    dict++;
                }
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
        data_vec[i].second->set_dict(i + 1);
    }
    return Compute(data_pool, column);
}

int AssignByFrontFreq(std::map<std::string, DataAttr*> &data_pool, std::vector<std::pair<std::string, std::set<std::string>>> &column) {
    std::vector<std::pair<std::string, DataAttr*>> data_vec;
    std::copy(data_pool.begin(), data_pool.end(), std::back_inserter<std::vector<std::pair<std::string, DataAttr*>>>(data_vec));
    assert(data_vec.size() == data_pool.size());
    std::sort(data_vec.begin(), data_vec.end(), [](const std::pair<std::string, DataAttr*> &data1, const std::pair<std::string, DataAttr*> &data2){
        if (data1.second->get_front_freq() != data2.second->get_front_freq()) {return data1.second->get_front_freq() < data2.second->get_front_freq();}
        return data1.first < data2.first;
    });
    for(int i = 0; i < data_vec.size(); i++) {
        data_vec[i].second->set_dict(data_vec.size() - i);
    }
    return Compute(data_pool, column);
}

int main(int argc, char *argv[]) {
    int data_set = std::atoi(argv[1]);
    std::printf("data_%d.txt\t", data_set);
    std::vector<std::pair<std::string, std::set<std::string>>> column;
    column.resize(BLOCK_NUM);
    std::map<std::string, DataAttr*> data_pool;
    std::map<int, OuterList> data_list;
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