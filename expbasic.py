import os
import argparse
import math
import random
import subprocess
import re
import numpy as np
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='main_runner')
parser.add_argument('--num_exp','-ne',type=int,default=10,help='Number of experiments')
parser.add_argument('--mod','-m',type=int,default=0,help='Paramter to modulate (0 for n, 1 for epsilon, 2 for m, 3 for s, 4 for range, 5 for B)')

paramnames = ['Dataset Size', 'Buckets to Datset Size Ratio', 'Maximum Possible Value', 'Range']
paramshortnames = ['N','c','M','range']
resultnames = ['Processing Time','False Positive Rate','Memory']

args = parser.parse_args()
ne = args.num_exp
mod = paramshortnames[int(args.mod)]

with open('expbasic/expbasic_%s.txt'%mod,'r') as f:
	lines = f.readlines()

params = np.zeros((4,len(lines)))
results = np.zeros((3,len(lines)))

i = 0

subprocess.check_call("make clean",shell=True)
subprocess.check_call("make all",shell=True)
subprocess.check_call("chmod u+x test_immutable.o",shell=True)

for line in lines:
	print(line)
	params_now = line.split(' ')
	params[0][i] = float(params_now[0])
	params[1][i] = float(params_now[1])
	params[2][i] = float(params_now[2])
	params[3][i] = float(params_now[3])

	for exp in range(ne):
		print(exp)
		output = subprocess.check_output("python mainBasic.py -n %s -c %s -m %s -r %s"%tuple(line.split(' ')),shell=True)
		output = output.decode('utf-8')
		output = output[2:-2]

		splitoutput = output.split('\n')
		results[0][i] += (1./ne)*float(splitoutput[0].split(' ')[1])
		results[1][i] += (1./ne)*float(splitoutput[1].split(' ')[1])
		results[2][i] += (1./ne)*float(splitoutput[2].split(' ')[1])

	i+=1

param_vari = int(args.mod) # np.argmax(abs(params[:,0]-params[:,1]))

for j in range(3):
	plt.figure()
	plt.plot(params[param_vari],results[j])
	plt.xlabel(paramnames[param_vari])
	plt.ylabel(resultnames[j])
        f = r'expbasic/imgs/%s/basic_%s_%s.png'%(paramshortnames[param_vari],resultnames[j],paramshortnames[param_vari])
        os.system('touch ' + f)
	plt.savefig(f)

finalresults = results.transpose()
np.savetxt(r'expbasic/imgs/%s/basic_results_%s.csv'%(paramshortnames[param_vari],paramshortnames[param_vari]), finalresults, delimiter=',')
