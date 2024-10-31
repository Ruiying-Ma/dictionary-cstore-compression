#include <bits/stdc++.h>
int BLOCK_SIZE = 1000;
int BLOCK_NUM = 1000;
int NDV = 10000;

void gen_data(double purity, std::vector<int> &column, double mean, double stddev) {
    std::printf("%.6lf\t", stddev);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> normal_distribution(mean, stddev);

    std::vector<int> max_slots_sizes(BLOCK_NUM, 1);
    int remainings = (int)(purity * (double)BLOCK_NUM * (double)BLOCK_SIZE) - BLOCK_NUM;
    double sum = 0;
    double zipf_power = 1;
    for(int i = 1; i <= BLOCK_NUM; i++) {
        sum += (double)1 / pow((double)i, zipf_power);
    }
    double unit = purity * (double)BLOCK_NUM * (double)BLOCK_SIZE / sum;
    for(int block = 0; block < BLOCK_NUM; block++) {
        assert(remainings >= 0);
        if (remainings == 0) {continue;}
        int max_slot_size = (int)(unit / pow((double)(block + 1), zipf_power));
        if (max_slot_size >= BLOCK_SIZE) {
            max_slot_size = BLOCK_SIZE;
        }
        if (remainings + 1 < max_slot_size) {
            max_slot_size = remainings + 1;
        }
        max_slots_sizes[block] = max_slot_size;
        assert(max_slots_sizes[block] >= 1 && max_slots_sizes[block] <= BLOCK_SIZE);
        remainings = remainings + 1 - max_slot_size;
    }
    int b = 0;
    while (remainings > 0) {
        if (max_slots_sizes[b] < BLOCK_SIZE) {
            int delta = std::min(BLOCK_SIZE - max_slots_sizes[b], remainings);
            max_slots_sizes[b] += delta;
            remainings -= delta;
        }
        b++;
    }
    assert(remainings == 0);
    // check
    int max_slot_sizes_sum = 0;
    for(int block = 0; block < BLOCK_NUM; block++) {
        assert(max_slots_sizes[block] >= 1 && max_slots_sizes[block] <= BLOCK_SIZE);
        max_slot_sizes_sum += max_slots_sizes[block];
    }
    assert(max_slot_sizes_sum == (int)(purity * (double)BLOCK_NUM * (double)BLOCK_SIZE));

    std::vector<std::vector<int>> value_block;
    value_block.resize(NDV);
    for(int i = 0; i < NDV; i++) {
        value_block[i].resize(BLOCK_NUM);
        for(int block = 0; block < BLOCK_NUM; block++) {
            value_block[i][block] = max_slots_sizes[block];
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
        for(int offset = 0; offset < max_slots_sizes[block]; offset++) {
            column[block * BLOCK_SIZE + offset] = max_slot_value;
        }
    }

    std::vector<int> block_offsets;
    int first_nonfull_block = 0;
    block_offsets.resize(BLOCK_NUM);
    for(int block = 0; block < BLOCK_NUM; block++) {
        block_offsets[block] = max_slots_sizes[block];
        if(max_slots_sizes[block] == BLOCK_SIZE) {first_nonfull_block = block;}
    }
    
    first_nonfull_block++;
    std::uniform_int_distribution<int> uniform_distribution(first_nonfull_block, BLOCK_NUM - 1);

    for(int value_idx = 0; value_idx < NDV; value_idx++) {
        if (!max_slot_values[value_idx]) {
            int block = uniform_distribution(generator);
            assert(block >= 0 && block < BLOCK_NUM);
            int offset = block_offsets[block];
            assert(offset >= max_slots_sizes[block] && offset <= BLOCK_SIZE);
            while(offset == BLOCK_SIZE) {
                int block = uniform_distribution(generator);
                assert(block >= 0 && block < BLOCK_NUM);
                int offset = block_offsets[block];
                assert(offset >= max_slots_sizes[block] && offset <= BLOCK_SIZE);
            }
            assert(offset >= max_slots_sizes[block] && offset < BLOCK_SIZE);
            column[block * BLOCK_SIZE + offset] = value_idx + 1;
            block_offsets[block]++;
            assert(value_block[value_idx][block] == max_slots_sizes[block]);
            value_block[value_idx][block]--;
        }
    }

    for(int block = 0; block < BLOCK_NUM; block++) {
        int offset = block_offsets[block];
        assert(offset >= max_slots_sizes[block] && offset <= BLOCK_SIZE);
        while (offset != BLOCK_SIZE) {
            assert(column[block * BLOCK_SIZE + offset] == 0);
            int value = (int)normal_distribution(generator);
            while (value <= 0 || value > NDV || value_block[value - 1][block] == 0) {
                value = (int)normal_distribution(generator);
            }
            assert(value >= 1 && value <= NDV && value_block[value - 1][block] > 0);
            column[block * BLOCK_SIZE + offset] = value;
            block_offsets[block]++;
            value_block[value - 1][block]--;
            offset++;
        }
        assert(block_offsets[block] == BLOCK_SIZE);
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
    double purity = 0.9;
    std::vector<int> column (BLOCK_NUM * BLOCK_SIZE, 0);
    gen_data(purity, column, (double)mean, (double)stddev);
    write_data(purity, column);
    return 0;
}