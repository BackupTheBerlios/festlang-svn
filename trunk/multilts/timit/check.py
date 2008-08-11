#!/usr/bin/python

infile = open ("lex.result")
result = open ("n-best.result")

n = 4
correct = [0,0,0,0,0,0,0,0,0,0,0,0,0]
count = 0

for line in infile:
    trans = line.strip().replace("_epsilon_ ", "").replace(" _epsilon_", "")
    count = count + 1
    i = 0
    found = False
    while True:
	res = result.readline().strip().replace("_epsilon_ ", "").replace(" _epsilon_", "")
	if res == "----":
	    break;
	if res == trans and not(found):
	    correct[i] = correct[i] + 1
	    found = True
#	else:
#	    print res
	i = i + 1
    if found == False:
	print trans

res = 0
print correct 
for i in range(1,n):
    res = 0.0
    for j in range(0,i):
	res = res + correct[j]
    print i, res / count