from fileinput import filename
from this import d
from tokenize import String
from turtle import dot
import matplotlib.pyplot as plt
import numpy as np

def plot_data (data, purity, ndv, is_purity):
    if is_purity:
        dot_color = "red"
        dot_label = "purity = 0.2"
        if purity == "0.400000":
            dot_color = "orange"
            dot_label = "purity = 0.4"
        elif purity == "0.600000":
            dot_color = "blue"
            dot_label = "purity = 0.6"
        elif purity == "0.800000":
            dot_color = "green"
            dot_label = "purity = 0.8"
        elif purity == "0.900000":
            dot_color = "purple"
            dot_label = "purity = 0.9"
    else:
        dot_color = "red"
        dot_label = "ndv = 10000"
        if ndv == 1000:
            dot_color = "orange"
            dot_label = "ndv = 1000"
        elif ndv == 200:
            dot_color = "blue"
            dot_label = "ndv = 200"

    stddev = data[1:, 0]
    variance = data[1:, 3]
    entropy = data[1:, 4]
    kurtosis = data[1:, 5]
    freq_rand = data[1:, 6]
    plt.subplot(231)
    plt.scatter(stddev, variance, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("stddev")
    plt.ylabel("variance")
    plt.legend(loc=1, prop={'size':6})
    plt.subplot(232)
    plt.scatter(stddev, entropy, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("stddev")
    plt.ylabel("entropy")
    plt.subplot(233)
    plt.scatter(stddev, kurtosis, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("stddev")
    plt.ylabel("kurtosis")

    plt.subplot(234)
    plt.scatter(variance, freq_rand, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("variance")
    plt.ylabel("freq/rand")
    plt.subplot(235)
    plt.scatter(entropy, freq_rand, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("entropy")
    plt.ylabel("freq/rand")
    plt.subplot(236)
    plt.scatter(kurtosis, freq_rand, s=1.0, color=dot_color, label=dot_label)
    plt.xlabel("kurtosis")
    plt.ylabel("freq/rand")


cnt = 1
print("same ndv, different purities")
for i in range(3):
    plt.figure(cnt)
    ndv = 10000
    if i == 1:
        ndv = 1000
    elif i == 2:
        ndv = 200

    for purity in ["0.200000", "0.400000", "0.600000", "0.800000", "0.900000"]:
        src_name = "purity_" + purity + "/bp_test" + str(i + 1) + ".txt"
        print(src_name)
        data = np.loadtxt(src_name)
        plot_data(data, purity, ndv, True)

    plt.suptitle("zipf-purity, ndv = " + str(ndv))
    plt.tight_layout()
    plt.savefig("ndv = " + str(ndv) + ".png")
    cnt += 1

print("same purity, different ndvs")
for purity in ["0.200000", "0.400000", "0.600000", "0.800000", "0.900000"]:
    plt.figure(cnt)
    for i in range(3):
        ndv = 10000
        if i == 1:
            ndv = 1000
        elif i == 2:
            ndv = 200

        src_name = "purity_" + purity + "/bp_test" + str(i + 1) + ".txt"
        print(src_name)
        data = np.loadtxt(src_name)
        plot_data(data, purity, ndv, False)

    plt.suptitle("zipf-purity, purity = " + purity[0:3])
    plt.tight_layout()
    plt.savefig("purity =  " + purity[0:3] + ".png")
    cnt += 1