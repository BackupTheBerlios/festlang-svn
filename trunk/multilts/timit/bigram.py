#!/usr/bin/python

file = open("bigram.data", "r")

bigrams={}

count = 0
for line in file:
    prev = "#"
    for l in line.split():
	count = count + 1
	key = prev + "-" + l
	if bigrams.has_key(key):
	    bigrams[key] = bigrams[key] + 1
	else:
	    bigrams[key] = 1.0
	prev = l

for key in bigrams.keys():
    bigrams[key] = bigrams[key] / count

print "bigram = "
print bigrams
