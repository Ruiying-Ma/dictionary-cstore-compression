#!/bin/bash


echo  > test3.txt

# data_i.txt    ndv     build_data_time     ratio   compute_time    total_time

for data_set in {0..29}
do
    timeout 20s ./test3 $data_set >> test3.txt
done