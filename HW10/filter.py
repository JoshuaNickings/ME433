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

openAppend("sigA.csv", tA, dataA)
openAppend("sigB.csv", tB, dataB)
openAppend("sigC.csv", tC, dataC)
openAppend("sigD.csv", tD, dataD)

def sampleRate(t):
        samp_rate = (len(t)) / (t[-1] - t[0])
        return samp_rate

def generate_fft(input_t, input_data):
    dt = sampleRate(input_t) ** -1
    t = np.arange(input_t[0], input_t[-1], dt) # 10s; "dt" is the step time which is 1 over the sample rate
    # a constant plus 100Hz and 1000Hz
    s = input_data

    Fs = sampleRate(input_t) # sample rate, time interval between first and last data point divided by total number of data points
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,y,'b')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()

generate_fft(tA, dataA)
generate_fft(tB, dataB)
generate_fft(tC, dataC)
generate_fft(tD, dataD)