from turtle import title
import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("purity_0.200000/bp_test3.txt")
stddev = data[1:, 0]
variance = data[1:, 3]
entropy = data[1:, 4]
kurtosis = data[1:, 5]
freq_rand = data[1:, 6]


plt.subplot(231)
plt.scatter(stddev, variance, s=1.0)
plt.xlabel("stddev")
plt.ylabel("variance")
plt.subplot(232)
plt.scatter(stddev, entropy, s=1.0)
plt.xlabel("stddev")
plt.ylabel("entropy")
plt.subplot(233)
plt.scatter(stddev, kurtosis, s=1.0)
plt.xlabel("stddev")
plt.ylabel("kurtosis")

plt.subplot(234)
plt.scatter(variance, freq_rand, s=1.0)
plt.xlabel("variance")
plt.ylabel("freq/rand")
plt.subplot(235)
plt.scatter(entropy, freq_rand, s=1.0)
plt.xlabel("entropy")
plt.ylabel("freq/rand")
plt.subplot(236)
plt.scatter(kurtosis, freq_rand, s=1.0)
plt.xlabel("kurtosis")
plt.ylabel("freq/rand")
plt.suptitle("ndv = 200, purity = 0.2")
plt.tight_layout()
plt.savefig("purity_0.200000/ndv=200.png")
#plt.show()
plt.close()