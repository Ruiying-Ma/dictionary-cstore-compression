import matplotlib.pyplot as plt
import numpy as np

def plot_data (data, is_unfiorm):
    dot_color = "red"
    dot_label = "uniform"
    if is_unfiorm == False:
        dot_color = "blue"
        dot_label = "zipf"

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

ndv_dict = {1: 10000, 2: 1000, 3: 200}

cnt = 1
for ndv_idx, ndv in ndv_dict.items():
    for purity in ["0.200000", "0.400000", "0.600000", "0.800000", "0.900000"]:
        uniform_data = np.loadtxt("uniform_purity/purity_" + purity + "/bp_test" + str(ndv_idx) + ".txt")
        zipf_data = np.loadtxt("zipf_purity/purity_" + purity + "/bp_test" + str(ndv_idx) + ".txt")
        plt.figure(cnt)
        # plot
        plot_data(uniform_data, True)
        plot_data(zipf_data, False)

        plt.suptitle("ndv = " +  str(ndv) + ", purity = " + purity[0 : 3])
        plt.tight_layout()
        plt.savefig("results/" + str(ndv) + "-" + purity[0 : 3] + ".png")
        cnt += 1



