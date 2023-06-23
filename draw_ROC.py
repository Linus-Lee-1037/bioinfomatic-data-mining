import pylab
from pylab import plt
import numpy as np

txt = open('./SVM_Sn_Sp.txt', 'r').readlines()
Sn = []
Sp = []
for line in txt[1:]:
    Sn.append(float(line.rstrip().split('\t')[0]))
    Sp.append(float(line.rstrip().split('\t')[1]))
font = {'family': 'arial',
        'weight': 'bold',
        'size': 13}
params = {'axes.labelsize': '20',
          'xtick.labelsize': '20',
          'ytick.labelsize': '20',
          'lines.linewidth': '4'}
pylab.rcParams.update(params)
pylab.rcParams['font.family'] = 'sans-serif'
pylab.rcParams['font.sans-serif'] = ['Arial']
plt.figure(figsize=(5, 5), dpi=300)
plt.plot(Sp, Sn, linewidth='2', label='SVM', color='salmon')
plt.plot([0, 1], [0, 1], linewidth='1', color='grey', linestyle="--")
plt.yticks(np.linspace(0.7, 1, 6), size=8)
plt.xticks(np.linspace(0.7, 1, 6), size=8)
plt.xlim((0.7, 1))
plt.ylim((0.7, 1))
plt.legend(prop={'size': 13}, loc="lower right", frameon=False)
plt.xlabel('Specificity', font)
plt.ylabel('Sensitivity', font)
plt.savefig('./SVM_Sn-Sp.png')
