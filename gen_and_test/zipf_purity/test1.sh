#!/bin/bash

echo > purity_0.900000/bp_test1.txt

stddev=50

while [ $stddev -le 100 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.900000/bp_test1.txt
    ./test1 >> purity_0.900000/bp_test1.txt
    ((stddev += 1))
done

exit 1

while [ $stddev -le 1000 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.900000/bp_test1.txt
    ./test1 >> purity_0.900000/bp_test1.txt
    ((stddev += 10))
done

while [ $stddev -le 6001 ]
do
    echo $stddev
    ./gen_data $stddev >> purity_0.900000/bp_test1.txt
    ./test1 >> purity_0.900000/bp_test1.txt
    ((stddev += 100))
done