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
        '''計算兩個點的距離'''

        return (v1[0] - v2[0])**2 + (v1[1] - v2[1])**2

    def cluster(self, centers):
        '''將每個點分配到最近的中心'''

        newgroup = [[] for i in range(self.k)]
        for point in self.data:
            mindis = self.calc_dis(point, centers[0])  # 計算距離
            mingroup = 0
            for k in range(1, self.k):
                newdis = self.calc_dis(point, centers[k])  # 計算距離
                if newdis < mindis:  # 如果較短
                    mindis = newdis  # 更新最近所屬中心
                    mingroup = k
            newgroup[mingroup].append(point)  # 將點分配到最近中心

        return newgroup

    def calc_group_center(self, group):
        '''計算各群群中心'''

        return [np.mean(np.array(group[i]), axis=0) for i in range(self.k)]

    def run(self, inputpath):
        '''主函數'''

        starttime = time.time()

        filename = os.path.splitext(inputpath)[0]

        # 讀CSV檔
        self.data = []
        with open(inputpath) as csvfile:
            csvreader = csv.reader(csvfile)
            for row in csvreader:
                self.data.append((float(row[0]), float(row[1])))

        # 隨機選取最初群中心
        centers = []
        while len(centers) < self.k:
            i = random.randint(0, len(self.data) - 1)
            if tuple(self.data[i]) not in centers:  # 防止選到重疊的點
                centers.append(tuple(self.data[i]))

        oldgroup = self.cluster(centers)  # 計算各點所屬群組

        step = 1
        while True:
            logging.info('step %s', step)
            centers = self.calc_group_center(oldgroup)  # 計算群中心
            logging.info('centers %s', centers)

            newgroup = self.cluster(centers)  # 計算各點所屬群組

            # 如果沒有更動則退出迴圈
            if oldgroup == newgroup:
                logging.info('Nothing to do')
                break

            # 可以限制計算次數，資料很大才會用到
            if self.limit != 0 and step >= self.limit:
                logging.info('Limit')
                break

            # 將新群覆蓋舊群，繼續所有步驟
            oldgroup = newgroup
            step += 1

        spendtime = time.time() - starttime
        logging.info('It tooks %s seconds. %s seconds per step.',
                     spendtime, spendtime / step)

        # 顯示點分群狀態
        colors = 'bgrcmyk'
        for gid, group in enumerate(newgroup):
            for point in group:
                # print(gid, point, colors[gid])
                plt.plot(point[0], point[1], colors[gid], marker='o')
        plt.savefig('{}.jpg'.format(filename))  # 存檔至 /data 資料夾
        plt.show()


if __name__ == '__main__':
    # 程式進入點
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(levelname)s %(message)s')

    '''命令列參數
    input: 輸入CSV檔路徑
    k: 分的群數
    limit: 如果檔案很大，可以限制計算次數
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('input', nargs='?', default='data/1.csv')
    parser.add_argument('--k', type=int, default=3)
    parser.add_argument('--limit', type=int, default=0)
    args = parser.parse_args()
    print(args)

    # 初始化分群物件
    kmeans = KMeans(args.k)
    kmeans.limit = args.limit
    kmeans.run(args.input)
