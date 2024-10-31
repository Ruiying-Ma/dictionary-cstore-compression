#!/bin/bash

echo > ../Data/ml_data/bp_test1.txt

for data_set in {0..29}
do
    ./test1 $data_set >> ../Data/ml_data/bp_test1.txt
done