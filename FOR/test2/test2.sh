#!/bin/bash

echo > ../../Data/ml_data/for_test2.txt

for data_set in {0..29}
do
    timeout 300s ./test2 $data_set >> ../../Data/ml_data/for_test2.txt
done