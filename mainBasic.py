import os
import argparse
import math
import random
import subprocess
import time

parser = argparse.ArgumentParser(description='main_runner')
parser.add_argument('--dataset_size','-n',type=int,default=1000,help='Number of elements in S, i.e. number of key-value pairs')
parser.add_argument('--c','-c',type=float,default=2.5,help='Number of buckets')
parser.add_argument('--maxVal','-m',type=int,default=130,help='Max Value of function output')
parser.add_argument('--range','-r',type=int,default=20,help='Number of possible function output values')
args = parser.parse_args()

n = args.dataset_size
c = args.c
maxVal = args.maxVal
r = args.range

k = int(math.ceil(math.log(r, 2)))

if maxVal < r:
	raise ValueError('Specify maxVal to be larger than or equal range.')

#if c < 2:
#	raise ValueError('c must be at least 2')

keys = random.sample(range(n), n)
data = [random.randint(0,r-1) for i in range(n)]

with open('dataset.txt', 'w') as f:
    for i in range(len(data)):
        key = keys[i]
        item = data[i]
        f.write("%s %s\n" % (key,item) )

print(subprocess.check_output("./test_immutable.o %s %s %s %s < dataset.txt"%(n,c,maxVal,k),shell=True))

