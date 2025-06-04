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

def generate_fft(input_t, input_data, color_code, ax1, ax2):
    dt = sampleRate(input_t) ** -1
    t = np.arange(input_t[0], input_t[-1], dt) # 10s; "dt" is the step time which is 1 over the sample rate
    # a constant plus 100Hz and 1000Hz
    t = t[:len(input_data)]
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

    ax1.plot(t,y,'b', color=color_code)
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'b', color=color_code) # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')

def maf(t, data, avg_num):
    maf_t = []
    maf_data = []
    for i in range(len(data)):
        if i < avg_num - 1:
            continue
        sum = 0
        for j in range(avg_num):
            sum += data[i - j]
        val = sum/avg_num

        maf_t.append(t[i])
        maf_data.append(val)
    return [maf_t, maf_data]

avg_num_A = 700
maf_A = maf(tA, dataA, avg_num_A) # 10<, 100<, 1000>, 500<, 750>, 650<, 700=
maf_tA = maf_A[0] # column 0
maf_dataA = maf_A[1] # column 1

avg_num_B = 150
maf_B = maf(tB, dataB, avg_num_B) # 10<, 100<, 1000>, 500>, 250>, 150=
maf_tB = maf_B[0] # column 0
maf_dataB = maf_B[1] # column 1

avg_num_C = 10
maf_C = maf(tC, dataC, avg_num_C)
maf_tC = maf_C[0] # column 0
maf_dataC = maf_C[1] # column 1

avg_num_D = 25
maf_D = maf(tD, dataD, avg_num_D) # 10<, 100>, 50>, 25=
maf_tD = maf_D[0] # column 0
maf_dataD = maf_D[1] # column 1

fig, (ax1, ax2) = plt.subplots(2, 1)
generate_fft(tA, dataA, 'k', ax1, ax2)
generate_fft(maf_tA, maf_dataA, 'r', ax1, ax2)
plt.title("A: Data Points Averaged " + str(avg_num_A), y = 2.2)

fig, (ax3, ax4) = plt.subplots(2, 1)
generate_fft(tB, dataB, 'k', ax3, ax4)
generate_fft(maf_tB, maf_dataB, 'r', ax3, ax4)
plt.title("B: Data Points Averaged " + str(avg_num_B), y = 2.2)

fig, (ax5, ax6) = plt.subplots(2, 1)
generate_fft(tC, dataC, 'k', ax5, ax6)
generate_fft(maf_tC, maf_dataC, 'r', ax5, ax6)
plt.title("C: Data Points Averaged " + str(avg_num_C), y = 2.2)

fig, (ax7, ax8) = plt.subplots(2, 1)
generate_fft(tD, dataD, 'k', ax7, ax8)
generate_fft(maf_tD, maf_dataD, 'r', ax7, ax8)
plt.title("D: Data Points Averaged " + str(avg_num_D), y = 2.2)
plt.show()
