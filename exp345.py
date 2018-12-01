import os
import argparse
import math
import random
import subprocess
import re
import numpy as np
import matplotlib.pyplot as plt

with open('exp345.txt','r') as f:
	lines = f.readlines()

params = np.zeros((5,len(lines)))
results = np.zeros((6,len(lines)))

i = 0

for line in lines:
	params_now = line.split(' ')
	params[0][i] = float(params_now[0])
	params[1][i] = float(params_now[1])
	params[2][i] = float(params_now[2])
	params[3][i] = float(params_now[3])
	params[4][i] = float(params_now[4])

	output = subprocess.check_output("python main.py -n %s -e %s -m %s -s %s -r %s"%tuple(line.split(' ')),shell=True)
	output = output.decode('utf-8')
	output = output[2:-2]

	splitoutput = output.split(' ')
	results[0][i] = float(splitoutput[1][:-22])
	results[1][i] = float(splitoutput[2][:-28])
	results[2][i] = float(splitoutput[3][:-15])
	results[3][i] = float(splitoutput[4][:-19])
	results[4][i] = float(splitoutput[5][:-18])
	results[5][i] = float(splitoutput[6][:-2])

	i+=1

param_vari = np.argmax(abs(params[:,0]-params[:,1]))

paramnames = ['Dataset Size','Epsilon','Number of Bits of p','Number of Hash Functions','Range of f']
resultnames = ['One-sided Processing Time','False Positive Rate','Two-sided Processing Time','Error Rate (in S)','Error Rate (not in S)','Number of Hash Blocks']

for j in range(6):
	plt.figure()
	plt.plot(params[param_vari],results[j])
	plt.xlab(paramnames[param_vari])
	plt.ylab(resultnames[j])
	plt.show()

