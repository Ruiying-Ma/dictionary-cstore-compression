#!/bin/bash

echo > ../SmallData/small_ml_data/bp_test1.txt

for data_set in {0..29}
do
    echo $data_set
    timeout 100s ./small_test1 $data_set >> ../SmallData/small_ml_data/bp_test1.txt
done