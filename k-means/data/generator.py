import csv
import numpy as np

data = (np.random.rand(100, 2) * 50).astype(np.int)
with open('output.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerows(data)
