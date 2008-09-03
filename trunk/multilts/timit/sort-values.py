#!/usr/bin/python

import math

f = open ("values", "r");

def numeric_compare(x, y):
    if float(x[1])>float(y[1]):
       return -1
    elif float(x[1])==float(y[1]):
       return 0
    else: # x<y
       return 1


for line in f:
    variants = []
    words = line.strip().replace("(", "").replace(")", "").replace(",","").replace("\"", "").split()
    if len(words) < 2: 
	continue;
    i = 0
    while i < len(words):
	variants.append ([words[i], words[i+1]])
	i = i + 2
    while len(variants) < 3:
	variants.append (['0', '0.0000001'])
    variants.sort (numeric_compare)
    for v in variants[0:3]:
	print "{ ", v[0],  ", ", int(math.log(float(v[1]))/math.log(1.003)), "},"
