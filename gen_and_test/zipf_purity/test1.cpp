#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;

struct DataAttr {
    int m_freq = 1;
    int m_dict = -1;
};

double ReadData(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column, double &purity) {
    std::ifstream read_file;
    read_file.open("purity_0.900000/data.txt", std::ios::in);

    std::clock_t start = std::clock();
    int block = 0;
    int offset = 0;
    std::map<std::string, int> block_map;
    int block_max = -1;
    if (read_file.is_open()) {
        std::string data;
        while (std::getline(read_file, data)) {
            assert(block >=0 && block < BLOCK_NUM && offset >=0 && offset < BLOCK_SIZE);
            assert(column[block].size() >= 0 && column[block].size() <= BLOCK_SIZE && block_map.size() >=0 && block_map.size() <= BLOCK_SIZE);
            if (offset == 0) {assert(column[block].size() == 0); assert(block_map.size() == 0);}
            if (column[block].count(data) > 0) {
                assert(data_pool.count(data) > 0);
                assert(block_map.count(data) > 0);
                block_map[data]++;
                if (block_map[data] > block_max) {block_max = block_map[data];}
            } else {
                column[block].insert(data);
                block_map.emplace(data, 1);
                if (block_max < 1) {block_max = 1;}
                if (data_pool.count(data) > 0) {
                    data_pool[data]->m_freq++;
                } else {
                    DataAttr* data_attr = new DataAttr;
                    data_pool.emplace(data, data_attr);
                }
            }
            offset++;
            if (offset == BLOCK_SIZE) {
                assert(column[block].size() > 0);
                assert(block_map.size() > 0);
                assert(block_max > 0);
                purity += (double) block_max;
                block_max = -1;
                block_map.clear();
                offset = 0;
                block++;
            }
        } 
    } else {
        assert("Fail to open the file\n");
    }
    std::clock_t end = std::clock();

    read_file.close();
    purity /= (double)(BLOCK_NUM * BLOCK_SIZE);
    return (double)(end - start) / (double) CLOCKS_PER_SEC;

    // //////////
    // for(auto& data : data_pool) {
    //     std::cout<<"data = "<<data.first<<", freq = "<<data.second->m_freq<<"\n";
    // }
    // std::cout<<"ndv = "<<(double)data_pool.size() * (double)100 / (double)BLOCK_NUM / (double)BLOCK_SIZE<<"%\n";
    // std::cout<<"Finish Reaing\n";
    // ////////

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

int Compute(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
    CheckAssign(data_pool);
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

int AssignByRandom(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
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

int AssignByFreq(std::map<std::string, DataAttr*> &data_pool, std::vector<std::set<std::string>> &column) {
    std::vector<std::pair<std::string, DataAttr*>> data_vec;
    std::copy(data_pool.begin(), data_pool.end(), std::back_inserter<std::vector<std::pair<std::string, DataAttr*>>>(data_vec));
    assert(data_vec.size() == data_pool.size());
    std::sort(data_vec.begin(), data_vec.end(), [](const std::pair<std::string, DataAttr*> &data1, const std::pair<std::string, DataAttr*> &data2){
        if (data1.second->m_freq != data2.second->m_freq) {return data1.second->m_freq < data2.second->m_freq;}
        return data1.first < data2.first;
    });
    for(int i = 0; i < data_vec.size(); i++) {
        data_vec[i].second->m_dict = data_vec.size() - i;
    }
    return Compute(data_pool, column);
}

double Entropy(std::map<std::string, DataAttr*> &data_pool) {
    int freq_sum = 0;
    for(auto& data : data_pool) {
        freq_sum += data.second->m_freq;
    }
    double entropy = 0;
    for(auto& data : data_pool) {
        entropy += -((double) data.second->m_freq / (double) freq_sum) * log2((double) data.second->m_freq / (double) freq_sum);
    }
    return entropy;
}

double Kurtosis(std::map<std::string, DataAttr*> &data_pool, double mean, double variance) {
    double kurtosis = 0;
    for(auto& data : data_pool){
        kurtosis += pow((double)data.second->m_freq - mean, 4) / pow(variance, 2);
    }
    return kurtosis / (double)data_pool.size();
}


int main() {
    std::vector<std::set<std::string>> column;
    column.resize(BLOCK_NUM);
    std::map<std::string, DataAttr*> data_pool;
    double purity = 0;
    // Read data
    double read_data_time = ReadData(data_pool, column, purity);
    // Compute variance of freq
    double avg_freq = 0;
    for(auto& data : data_pool) {
        avg_freq += (double) data.second->m_freq;
    }
    avg_freq /= (double) data_pool.size();
    double variance = 0;
    for(auto& data : data_pool) {
        variance += ((double)data.second->m_freq - avg_freq) * ((double)data.second->m_freq - avg_freq);
    }
    variance /= (double)data_pool.size();
    variance = std::sqrt(variance);
    double kurtosis = Kurtosis(data_pool, avg_freq, variance);
    // Compute
    std::clock_t start = std::clock();
    double result_by_freq = (double) AssignByFreq(data_pool, column);
    std::clock_t end = std::clock();
    double ratio = 0;
    for(int i = 0; i < 10; i++) {
        double result_by_rand = (double) AssignByRandom(data_pool, column);
        ratio += result_by_freq / result_by_rand;
    }
    ratio /= 10;
    double ndv = (double)data_pool.size() * (double)100 / (double) BLOCK_NUM / (double) BLOCK_SIZE;
    double entropy = Entropy(data_pool);
    double compute_time = (double) (end - start) / (double)CLOCKS_PER_SEC;
    double total_time = read_data_time + compute_time;
    // Result
    std::printf("%.6lf\t", ndv);
    std::printf("%.6lf\t", purity);
    std::printf("%.6lf\t", variance);
    std::printf("%.6lf\t", entropy);
    std::printf("%.6lf\t", kurtosis);
    std::printf("%.6lf\t", ratio);
    std::printf("%.6lf\t", read_data_time);
    std::printf("%.6lf\t", compute_time);
    std::printf("%.6lf\n", total_time);

    // Release Meomry
    for(auto& data : data_pool) {
        delete data.second;
    }
    
}