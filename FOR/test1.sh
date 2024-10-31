#!/bin/bash

echo > ../Data/ml_data/for_test1.txt

for data_set in {0..29}
do
    timeout 100s ./test1 $data_set >> ../Data/ml_data/for_test1.txt
done