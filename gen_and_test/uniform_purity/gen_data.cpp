#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;
int NDV = 200;

void gen_data(double purity, std::vector<int> &column, double mean, double stddev) {

    std::printf("%.6lf\t", stddev);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> normal_distribution(mean, stddev);

    int max_slot_size = (int)(purity * (double)BLOCK_SIZE);

    std::vector<std::vector<int>> value_block;
    value_block.resize(NDV);
    for(auto& value : value_block) {
        value.resize(BLOCK_NUM);
    }
    for(int value = 0; value < NDV; value++) {
        for (int block = 0; block < BLOCK_NUM; block++) {
            value_block[value][block] = max_slot_size;
        }
    }

    std::vector<bool> max_slot_values(NDV, false);

    for(int block = 0; block < BLOCK_NUM; block++) {
        int max_slot_value = (int)normal_distribution(generator);
        while (max_slot_value <= 0 || max_slot_value > NDV) {
            max_slot_value = (int)normal_distribution(generator);
        }
        assert(max_slot_value >= 1 && max_slot_value <= NDV);
        value_block[max_slot_value - 1][block] = 0;
        max_slot_values[max_slot_value - 1] = true;
        for(int offset = 0; offset < max_slot_size; offset++) {
            column[block * BLOCK_SIZE + offset] = max_slot_value;
        }
    }

    std::vector<int> blocks_offsets (BLOCK_NUM, max_slot_size);
    std::uniform_int_distribution<int> uniform_distribution(0, BLOCK_NUM - 1);
    for(int value_idx = 0; value_idx < NDV; value_idx++) {
        if (!max_slot_values[value_idx]) {
            int block = uniform_distribution(generator);
            assert(block >= 0 && block < BLOCK_NUM);
            int offset = blocks_offsets[block];
            assert(offset >= max_slot_size && offset <= BLOCK_SIZE);
            while (offset == BLOCK_SIZE) {
                int block = uniform_distribution(generator);
                assert(block >= 0 && block < BLOCK_NUM);
                int offset = blocks_offsets[block];
                assert(offset >= max_slot_size && offset <= BLOCK_SIZE); 
            }
            assert(offset >= max_slot_size && offset < BLOCK_SIZE && column[block * BLOCK_SIZE + offset] == 0);
            column[block * BLOCK_NUM + offset] = value_idx + 1;
            blocks_offsets[block]++;
            assert(value_block[value_idx][block] == max_slot_size);
            value_block[value_idx][block]--;
        }
    }

    for(int block = 0; block < BLOCK_NUM; block++) {
        int offset = blocks_offsets[block];
        assert(offset >= max_slot_size && offset <= BLOCK_SIZE);
        while (offset != BLOCK_SIZE) {
            assert(column[block * BLOCK_SIZE + offset] == 0);
            int value = (int)normal_distribution(generator);
            while (value <= 0 || value > NDV || value_block[value - 1][block] == 0) {
                value = (int)normal_distribution(generator);
            }
            assert(value >= 1 && value <= NDV && value_block[value - 1][block] > 0);  
            column[block * BLOCK_SIZE + offset] = value;
            blocks_offsets[block]++;
            offset++;
            value_block[value - 1][block]--;
        }
        assert(blocks_offsets[block] == BLOCK_SIZE);
    }
    // Check
    for(auto& item : column) {
        assert(item >= 1 && item <= NDV);
    }    
}

void write_data(double purity, std::vector<int> &column) {
    std::ofstream write_file;
    std::string file_name = "purity_";
    file_name += std::to_string(purity);
    file_name += "/data.txt";
    write_file.open(file_name, std::ios::out);

    for(auto& data : column) {
        write_file << data <<"\n";
    }

    write_file.close();
}

int main(int argc, char *argv[]) {
    int mean = NDV / 2;
    int stddev = std::atoi(argv[1]);
    double purity = 0.2;
    std::vector<int> column (BLOCK_NUM * BLOCK_SIZE, 0);
    gen_data(purity, column, (double)mean, (double)stddev);
    write_data(purity, column);
    return 0;
}