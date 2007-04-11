#!/usr/bin/python

file = open("frequent-clusters", "r")

all_lines = file.readlines()

for line in all_lines:
    line = line.strip()
    found = 0
    for l in all_lines:
	l = l.strip()
	if l.find(line) >= 0 and line != l:
	    found = 1
    if not found:
	print line