from logging import setLoggerClass
import random

# uniform purity distribution: each block's max intersection is the same
BLOCK_NUM = int(1000)
BLOCK_SIZE = int(1000)
NDV = (int)(10000)  # %ndv = 1%, start from 1

purities = [0.2, 0.4, 0.6, 0.8, 0.9]

for purity in purities:
    max_slot_size = int(purities * BLOCK_SIZE)
    