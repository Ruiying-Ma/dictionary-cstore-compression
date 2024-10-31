#!/bin/bash

echo > ../Data/classic_data/delta_test1.txt

for data_set in {0..29}
do
    timeout 100s ./test1 $data_set >> ../Data/classic_data/delta_test1.txt
done