#!/usr/bin/python

for t in range(4,8):

    file = open ("words", "r")
    for line in file:
	phones = line.split()
        for i in range(t, len(phones) + 1):
    	    result = ""
	    for j in range(i-t,i):
		result += phones[j] + " " 
	    print result
    file.close()
