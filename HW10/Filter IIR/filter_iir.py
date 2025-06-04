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

def iir(t, data, a, b):
    iir_t = []
    iir_data = []
    sum = 0
    for i in range(len(data)):
        sum = (a / (a+b)) * sum + (b / (a+b)) * data[i]

        iir_t.append(t[i])
        iir_data.append(sum)
    return [iir_t, iir_data]

a_A = 0.9975
b_A = 0.0025
iir_A = iir(tA, dataA, a_A, b_A) # (0.5, 0.5), (0.75, 0.25), (0.9, 0.1),
# (0.95, 0.05), (0.99, 0.01), (0.999, 0.001), (0.995, 0.005), (0.9975, 0.0025)=
iir_tA = iir_A[0] # column 0
iir_dataA = iir_A[1] # column 1

a_B = 0.99
b_B = 0.01
iir_B = iir(tB, dataB, a_B, b_B) # (0.5, 0.5), (0.75, 0.25), (0.9, 0.1),
# (0.95, 0.05), (0.99, 0.01)=
iir_tB = iir_B[0] # column 0
iir_dataB = iir_B[1] # column 1

a_C = 0.5
b_C = 0.5
iir_C = iir(tC, dataC, a_C, b_C)
iir_tC = iir_C[0] # column 0
iir_dataC = iir_C[1] # column 1

a_D = 0.95
b_D = 0.05
iir_D = iir(tD, dataD, a_D, b_D) # (0.5, 0.5), (0.75, 0.25), (0.9, 0.1),
# (0.95, 0.05)=
iir_tD = iir_D[0] # column 0
iir_dataD = iir_D[1] # column 1

fig, (ax1, ax2) = plt.subplots(2, 1)
generate_fft(tA, dataA, 'k', ax1, ax2)
generate_fft(iir_tA, iir_dataA, 'r', ax1, ax2)
plt.title("A: Weight of a " + str(a_A) + " and b " + str(b_A), y = 2.2)

fig, (ax3, ax4) = plt.subplots(2, 1)
generate_fft(tB, dataB, 'k', ax3, ax4)
generate_fft(iir_tB, iir_dataB, 'r', ax3, ax4)
plt.title("B: Weight of a " + str(a_B) + " and b " + str(b_B), y = 2.2)

fig, (ax5, ax6) = plt.subplots(2, 1)
generate_fft(tC, dataC, 'k', ax5, ax6)
generate_fft(iir_tC, iir_dataC, 'r', ax5, ax6)
plt.title("C: Weight of a " + str(a_C) + " and b " + str(b_C), y = 2.2)

fig, (ax7, ax8) = plt.subplots(2, 1)
generate_fft(tD, dataD, 'k', ax7, ax8)
generate_fft(iir_tD, iir_dataD, 'r', ax7, ax8)
plt.title("D: Weight of a " + str(a_D) + " and b " + str(b_D), y = 2.2)
plt.show()
