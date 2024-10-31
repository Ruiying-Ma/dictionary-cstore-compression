#!/bin/bash

echo > purity_0.200000/bp_test3.txt

stddev=1

while [ $stddev -le 100 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.200000/bp_test3.txt
    ./test1 >> purity_0.200000/bp_test3.txt
    ((stddev += 1))
done

while [ $stddev -le 1000 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.200000/bp_test3.txt
    ./test1 >> purity_0.200000/bp_test3.txt
    ((stddev += 10))
done

while [ $stddev -le 6001 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.200000/bp_test3.txt
    ./test1 >> purity_0.200000/bp_test3.txt
    ((stddev += 100))
done