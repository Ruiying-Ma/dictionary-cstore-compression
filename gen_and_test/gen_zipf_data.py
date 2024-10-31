import numpy as np

import scipy.stats as stats

import matplotlib.pyplot as plt

import statistics

BLOCK_SIZE = 1000
BLOCK_NUM = 1000

N = BLOCK_SIZE

x = np.arange(1, N + 1)

a = 1.2

weights = x ** (-a)

weights /= weights.sum()

bounded_zipf = stats.rv_discrete(name='bounded_zipf', values=(x, weights))

sample = bounded_zipf.rvs(size = BLOCK_NUM)

print(statistics.mean(sample))

#plt.hist(sample, bins=np.arange(1, N+2))

#plt.show()