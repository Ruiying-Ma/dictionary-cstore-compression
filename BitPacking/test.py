import pandas as pd
import numpy as np
data = pd.read_csv('../OpenFormat-Public/benchmark/generator_v2/small_log_data/gen_data/small_log_data.csv', delimiter=',', dtype=str, header=None)

for col in range(30):
    data_col = np.array(data.iloc[:, col])

    file_name = "../SmallData/small_log_data/data_"
    file_name += str(col)
    file_name +=".txt"

    file = open(file_name, "w")

    for i in range(100):
        value = data_col[i]
        file.write(str(value))
        file.write("\n")

    file.close()