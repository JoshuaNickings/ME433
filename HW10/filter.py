import csv
import matplotlib.pyplot as plt
import numpy as np

tA = [] # column 0
dataA = [] # column 1

tB = [] # column 0
dataB = [] # column 1

tC = [] # column 0
dataC = [] # column 1

tD = [] # column 0
dataD = [] # column 1

def openAppend(file_name, t_list, data_list):
    with open(file_name) as f:
    # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t_list.append(float(row[0])) # leftmost column
            data_list.append(float(row[1])) # second column

def sampleRate(t):
        samp_rate = (len(t)) / (t[-1] - t[0])
        return samp_rate




