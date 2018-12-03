import os
import argparse
import math
import random
import subprocess

parser = argparse.ArgumentParser(description='main_runner')
parser.add_argument('--dataset_size','-n',type=int,default=1000,help='Number of elements in S, i.e. number of key-value pairs')
parser.add_argument('--epsilon','-e',type=float,default=0.1,help='Epsilon value')
parser.add_argument('--m','-m',type=int,default=15,help='The number of bits of the prime p')
parser.add_argument('--s','-s',type=int,default=4,help='Number of hash functions')
parser.add_argument('--range','-r',type=int,default=20,help='Number of possible function output values')
parser.add_argument('--num_exp','-ne',type=int,default=10,help='Number of experiments')
args = parser.parse_args()

n = args.dataset_size
e = args.epsilon
m = args.m
s = args.s
r = args.range

k = math.ceil(math.log(r, 2))

if m < k:
	raise ValueError('Specify m to be larger than or equal log_2(range).')

keys = random.sample(range(n*2), n)
data = [random.randint(0,r-1) for i in range(n)]

with open('dataset.txt', 'w') as f:
    for i in range(len(data)):
        key = keys[i]
        item = data[i]
        f.write("%s %s\n" % (key,item) )

subprocess.check_output("./test_improved.o %s %s %s %s %s < dataset.txt"%(n,e,m,s,k),shell=True)

