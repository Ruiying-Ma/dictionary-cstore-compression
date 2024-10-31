#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;
int NDV = 10000; // %ndv = 1%, {1, 2, ..., 10000}

void gen_data(double purity, std::vector<int> &column) {
    int max_slot_size = (int)(purity * (double)BLOCK_SIZE);
    for(int block = 0; block < BLOCK_NUM; block++) {
        std::vector<int> values_credits(NDV, max_slot_size);
        int max_slot_size_value = std::rand() % NDV + 1;
        assert(max_slot_size_value >= 1 && max_slot_size_value <= NDV);
        for(int offset = 0; offset < max_slot_size; offset++) {
            column[block * BLOCK_SIZE + offset] = max_slot_size_value;
            values_credits[max_slot_size_value - 1]--;
        }
        assert(values_credits[max_slot_size_value - 1] == 0);
        for(int offset = max_slot_size; offset < BLOCK_SIZE; offset++) {
            int value = std::rand() % NDV + 1;
            assert(values_credits[value - 1] >= 0);
            while (values_credits[value - 1] == 0) {
                value = std::rand() % NDV + 1;
                assert(values_credits[value - 1] >= 0);
            }
            column[block * BLOCK_SIZE + offset] = value;
            values_credits[value - 1]--;
        }
        // check
        for(int i = 0; i < NDV; i++) {assert(values_credits[i] >= 0);}
    }
}

void write_data(double purity, std::vector<int> &column, int data_set) {
    std::ofstream write_file;
    std::string file_name = "../Data/my_data/ndv_1_prct/uniform_purity/purity_";
    file_name += std::to_string(purity);
    file_name += "/";
    file_name += std::to_string(data_set);
    file_name += ".txt";
    write_file.open(file_name, std::ios::out);

    for(auto& data : column) {
        write_file << data <<"\n";
    }

    write_file.close();
}

int main() {
    std::vector<double> purities = {0.2, 0.4, 0.6, 0.8, 0.9};
    for(auto& purity : purities) {
        for(int data_set = 0; data_set < 1000; data_set++) {
            std::vector<int> column;
            column.resize(BLOCK_SIZE * BLOCK_NUM);
            gen_data(purity, column);
            write_data(purity, column, data_set);
        }
    }
    return 0;
}