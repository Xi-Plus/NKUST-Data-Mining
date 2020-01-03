import argparse
import csv
import logging
import os
import random
import time

import matplotlib.pyplot as plt
import numpy as np


class KMeans():
    limit = 0
    data = []

    def __init__(self, k):
        self.set_k(k)

    def set_k(self, k):
        self.k = k

    def calc_dis(self, v1, v2):
        return (v1[0] - v2[0])**2 + (v1[1] - v2[1])**2

    def cluster(self, centers):
        newgroup = [[] for i in range(self.k)]
        for point in self.data:
            mindis = self.calc_dis(point, centers[0])
            mingroup = 0
            for k in range(1, self.k):
                newdis = self.calc_dis(point, centers[k])
                if newdis < mindis:
                    mindis = newdis
                    mingroup = k
            newgroup[mingroup].append(point)

        return newgroup

    def calc_group_center(self, group):
        return [np.mean(np.array(group[i]), axis=0) for i in range(self.k)]

    def gen_img(self, height, width, oldgroup, centers):
        newimg = np.empty((height, width, 3), dtype=np.uint8)
        for i in range(self.k):
            for item in oldgroup[i]:
                newimg[item] = centers[i]
        return newimg

    def run(self, inputpath):
        starttime = time.time()

        filename = os.path.splitext(inputpath)[0]

        self.data = []
        with open(inputpath) as csvfile:
            csvreader = csv.reader(csvfile)
            for row in csvreader:
                self.data.append((float(row[0]), float(row[1])))

        centers = []
        while len(centers) < self.k:
            i = random.randint(0, len(self.data) - 1)
            if tuple(self.data[i]) not in centers:
                centers.append(tuple(self.data[i]))
        oldgroup = self.cluster(centers)
        step = 1
        while True:
            logging.info('step %s', step)
            centers = self.calc_group_center(oldgroup)
            logging.info('centers %s', centers)

            newgroup = self.cluster(centers)
            if oldgroup == newgroup:
                logging.info('Nothing to do')
                break

            if self.limit != 0 and step >= self.limit:
                logging.info('Limit')
                break

            oldgroup = newgroup
            step += 1

        spendtime = time.time() - starttime
        logging.info('It tooks %s seconds. %s seconds per step.',
                     spendtime, spendtime / step)

        # show points
        colors = 'bgrcmyk'
        for gid, group in enumerate(newgroup):
            for point in group:
                # print(gid, point, colors[gid])
                plt.plot(point[0], point[1], colors[gid], marker='o')
        plt.savefig('{}.jpg'.format(filename))
        plt.show()


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(levelname)s %(message)s')

    parser = argparse.ArgumentParser()
    parser.add_argument('input', nargs='?', default='data/1.csv')
    parser.add_argument('--k', type=int, default=3)
    parser.add_argument('--limit', type=int, default=0)
    parser.add_argument('--allimg', type=bool, default=False)
    args = parser.parse_args()
    print(args)

    kmeans = KMeans(args.k)
    kmeans.limit = args.limit
    kmeans.run(args.input)
