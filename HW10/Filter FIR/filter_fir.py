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
    dt = 5/50000
    t = np.arange(input_t[0], input_t[-1]+dt, dt) # 10s; "dt" is the step time which is 1 over the sample rate
    # a constant plus 100Hz and 1000Hz
    t = t[:len(input_data)]
    s = input_data

    Fs = 50000/5 # sample rate, (Inverse of what is about to be stated) time interval between first and last data point divided by total number of data points
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

def fir(t, data, coeffs):
    fir_t = []
    fir_data = []
    for i in range(len(data)):
        if i < len(coeffs) - 1:
            continue
        sum = 0
        for index in range(len(coeffs)):
            sum += data[i - len(coeffs) + index + 1] * coeffs[index]

        fir_t.append(t[i])
        fir_data.append(sum)
    return [fir_t, fir_data]

coeffs_A = [
    0.002704137524648795,
    0.002967016921491179,
    0.003601950466181234,
    0.004625363856653065,
    0.006041694357410096,
    0.007842815906846540,
    0.010007881373899290,
    0.012503596017112427,
    0.015284920835567576,
    0.018296188986998248,
    0.021472603363472408,
    0.024742069327748278,
    0.028027304059037649,
    0.031248153416712780,
    0.034324039112928278,
    0.037176453609771856,
    0.039731417741370081,
    0.041921816713873530,
    0.043689533848892161,
    0.044987308085996679,
    0.045780250613659659,
    0.046046967719456267,
    0.045780250613659666,
    0.044987308085996686,
    0.043689533848892161,
    0.041921816713873523,
    0.039731417741370081,
    0.037176453609771856,
    0.034324039112928292,
    0.031248153416712776,
    0.028027304059037656,
    0.024742069327748292,
    0.021472603363472412,
    0.018296188986998252,
    0.015284920835567570,
    0.012503596017112442,
    0.010007881373899295,
    0.007842815906846535,
    0.006041694357410104,
    0.004625363856653067,
    0.003601950466181236,
    0.002967016921491181,
    0.002704137524648795,
]

co_frq_A = 100 # Hz
trans_band_A = 750 # Hz
fir_A = fir(tA, dataA, coeffs_A) # 10<, 100<, 1000>, 500<, 750>, 650<, 700=
fir_tA = fir_A[0] # column 0
fir_dataA = fir_A[1] # column 1

coeffs_B = [
    0.000000000000000000,
    0.000006654468709502,
    0.000029128522936023,
    0.000071630294094485,
    0.000139045364715286,
    0.000237018146420107,
    0.000371990776314115,
    0.000551191427798928,
    0.000782566139698271,
    0.001074650878794299,
    0.001436383452247726,
    0.001876857948081446,
    0.002405027463171737,
    0.003029363833662816,
    0.003757485769150283,
    0.004595769074907365,
    0.005548954406576212,
    0.006619769140808002,
    0.007808580390891126,
    0.009113095905728782,
    0.010528128553079571,
    0.012045438326481696,
    0.013653663385294468,
    0.015338348625262888,
    0.017082076796941639,
    0.018864703378109852,
    0.020663692418049256,
    0.022454546570933227,
    0.024211320690754524,
    0.025907204835505290,
    0.027515159476891312,
    0.029008583268927898,
    0.030361992005490210,
    0.031551686475304340,
    0.032556386852028101,
    0.033357812050676970,
    0.033941184117168982,
    0.034295640137166114,
    0.034414537262454484,
    0.034295640137166114,
    0.033941184117168982,
    0.033357812050676984,
    0.032556386852028101,
    0.031551686475304340,
    0.030361992005490220,
    0.029008583268927898,
    0.027515159476891322,
    0.025907204835505290,
    0.024211320690754531,
    0.022454546570933234,
    0.020663692418049256,
    0.018864703378109866,
    0.017082076796941632,
    0.015338348625262895,
    0.013653663385294473,
    0.012045438326481693,
    0.010528128553079576,
    0.009113095905728785,
    0.007808580390891126,
    0.006619769140808005,
    0.005548954406576217,
    0.004595769074907367,
    0.003757485769150290,
    0.003029363833662813,
    0.002405027463171739,
    0.001876857948081447,
    0.001436383452247725,
    0.001074650878794302,
    0.000782566139698270,
    0.000551191427798929,
    0.000371990776314116,
    0.000237018146420106,
    0.000139045364715286,
    0.000071630294094485,
    0.000029128522936022,
    0.000006654468709502,
    0.000000000000000000,
]

co_frq_B = 100 # Hz
trans_band_B = 600 # Hz
fir_B = fir(tB, dataB, coeffs_B) # 10<, 100<, 1000>, 500>, 250>, 150=
fir_tB = fir_B[0] # column 0
fir_dataB = fir_B[1] # column 1

coeffs_C = [
    0.090011231804371733,
    0.090548754148121274,
    0.090968151967928110,
    0.091268433002908794,
    0.091448886490887463,
    0.091509085171565419,
    0.091448886490887463,
    0.091268433002908794,
    0.090968151967928110,
    0.090548754148121274,
    0.090011231804371733,
]

co_frq_C = 100 # Hz
trans_band_C = 1000 # Hz
fir_C = fir(tC, dataC, coeffs_C)
fir_tC = fir_C[0] # column 0
fir_dataC = fir_C[1] # column 1

coeffs_D = [
    0.325843353326760232,
    0.348313293346479480,
    0.325843353326760232,
]

co_frq_D = 1000 # Hz
trans_band_D = 4900 # Hz
fir_D = fir(tD, dataD, coeffs_D) # 10<, 100>, 50>, 25=
fir_tD = fir_D[0] # column 0
fir_dataD = fir_D[1] # column 1


fig, (ax1, ax2) = plt.subplots(2, 1)
generate_fft(tA, dataA, 'k', ax1, ax2)
generate_fft(fir_tA, fir_dataA, 'r', ax1, ax2)
plt.title("A: " + str(len(coeffs_A)) + " Hamming Coefficients, Cutoff Freq: " + str(co_frq_A) + ", Trans Band: " + str(trans_band_A), y = 2.2)

fig, (ax3, ax4) = plt.subplots(2, 1)
generate_fft(tB, dataB, 'k', ax3, ax4)
generate_fft(fir_tB, fir_dataB, 'r', ax3, ax4)
plt.title("B: " + str(len(coeffs_B)) + " Blackman Coefficients, Cutoff Freq: " + str(co_frq_B) + ", Trans Band: " + str(trans_band_B), y = 2.2)

fig, (ax5, ax6) = plt.subplots(2, 1)
generate_fft(tC, dataC, 'k', ax5, ax6)
generate_fft(fir_tC, fir_dataC, 'r', ax5, ax6)
plt.title("C: " + str(len(coeffs_C)) + " Rectangular Coefficients, Cutoff Freq: " + str(co_frq_C) + ", Trans Band: " + str(trans_band_C), y = 2.2)

fig, (ax7, ax8) = plt.subplots(2, 1)
generate_fft(tD, dataD, 'k', ax7, ax8)
generate_fft(fir_tD, fir_dataD, 'r', ax7, ax8)
plt.title("D: " + str(len(coeffs_D)) + " Rectangular Coefficients, Cutoff Freq: " + str(co_frq_D) + ", Trans Band: " + str(trans_band_D), y = 2.2)

plt.show()
