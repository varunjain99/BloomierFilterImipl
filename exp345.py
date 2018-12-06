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
parser.add_argument('--mod','-m',type=int,default=0,help='Paramter to modulate (0 for n, 1 for epsilon, 2 for m, 3 for s, 4 for range)')

paramnames = ['Dataset Size','Epsilon','Number of Bits of p','Number of Hash Functions','Range of f']
paramshortnames = ['n','epsilon','m','s','range']
resultnames = ['One-sided Processing Time','Number of Tries','False Positive Rate','Two-sided Processing Time','Error Rate (in S)','Error Rate (not in S)','Number of Hash Blocks']

args = parser.parse_args()
ne = args.num_exp
mod = paramshortnames[int(args.mod)]

with open('exp345/exp345_%s.txt'%mod,'r') as f:
	lines = f.readlines()

params = np.zeros((5,len(lines)))
results = np.zeros((7,len(lines)))

i = 0

subprocess.check_call("make clean",stdout=subprocess.DEVNULL,shell=True)
subprocess.check_call("make all",stdout=subprocess.DEVNULL,shell=True)
subprocess.check_call("chmod u+x test_improved.o",stdout=subprocess.DEVNULL,shell=True)

for line in lines:
	print(line)
	params_now = line.split(' ')
	params[0][i] = float(params_now[0])
	params[1][i] = float(params_now[1])
	params[2][i] = float(params_now[2])
	params[3][i] = float(params_now[3])
	params[4][i] = float(params_now[4])

	for exp in range(ne):
		print(exp)
		output = subprocess.check_output("python main.py -n %s -e %s -m %s -s %s -r %s"%tuple(line.split(' ')),shell=True)
		output = output.decode('utf-8')
		output = output[2:-2]

		splitoutput = output.split(' ')
		results[0][i] += (1./ne)*float(splitoutput[1].split('\\')[0])
		results[1][i] += (1./ne)*float(splitoutput[2].split('\\')[0])
		results[2][i] += (1./ne)*float(splitoutput[3].split('\\')[0])
		results[3][i] += (1./ne)*float(splitoutput[4].split('\\')[0])
		results[4][i] += (1./ne)*float(splitoutput[5].split('\\')[0])
		results[5][i] += (1./ne)*float(splitoutput[6].split('\\')[0])
		results[6][i] += (1./ne)*float(splitoutput[7].split('\\')[0])

	i+=1

param_vari = int(args.mod) # np.argmax(abs(params[:,0]-params[:,1]))

for j in range(7):
	plt.figure()
	plt.plot(params[param_vari],results[j])
	plt.xlabel(paramnames[param_vari])
	plt.ylabel(resultnames[j])
	plt.savefig('exp345/imgs/%s/345_%s_%s.png'%(paramshortnames[param_vari],resultnames[j],paramshortnames[param_vari]))

finalresults = results.transpose()
np.savetxt('exp345/imgs/%s/345_results_%s.csv'%(paramshortnames[param_vari],paramshortnames[param_vari]), finalresults, delimiter=',')

