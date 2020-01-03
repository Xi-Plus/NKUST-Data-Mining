import csv

import numpy as np
from sklearn import datasets


X1, y1 = datasets.make_circles(n_samples=500, factor=0.5, noise=.05, random_state=9)
X2, y2 = datasets.make_blobs(n_samples=100, n_features=2, centers=[[1.2, 1.2]], cluster_std=[[.1]], random_state=9)

dataset = np.concatenate((X1, X2))
dataset = (dataset * 100).astype(np.int)
with open('output.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerows(dataset)
