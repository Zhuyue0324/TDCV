import numpy as np
from numpy import *

def Loss(feats, batch_size):
    '''L_triplet'''
    m = 0.01
    '''should input the feature from output of CNN network, the following give some example for debugging'''
    #feats = np.array([[-4,1], [1, -4], [5, -2], [5, -5],[-2, 1], [2, -1]])
    #batch_size = 6
    diff_pos = feats[0: batch_size: 3] - feats[1: batch_size: 3]
    diff_neg = feats[0: batch_size: 3] - feats[1: batch_size: 3]
    '''calculate the Euclidean distance betwenn s_a and s_+'''
    diff_pos1 = zeros(len(diff_pos))
    for i in range(len(diff_pos)):
        for j in range(len(diff_pos[0])):
            diff_pos1[i] += np.square(diff_pos[i][j])
    '''calculate the Euclidean distance betwenn s_a and s_-'''
    diff_neg1 = zeros(len(diff_neg))
    for i in range(len(diff_neg)):
        for j in range(len(diff_neg[0])):
            diff_neg1[i] += np.square(diff_neg[i][j])
    Ltrip = []
    '''calculate the Loss of triplet'''
    for i in range(len(diff_neg1)):
        Ltrip.append(max(0,(1 - diff_neg1[i]/(np.square(diff_pos1[i] + m)))))
    L_trip = np.sum(Ltrip)
    '''calculate the Loss of pair-wise term'''
    L_pair = np.sum(diff_pos1)
    L = L_trip + L_pair
    return L