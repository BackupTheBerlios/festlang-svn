#!/usr/bin/python

import re



# cmudict to timit

replacement = {
    "ah0" : "ah",
    "ah1" : "ax",
    "er0" : "axr",
    "er1" : "er",
    "ih0" : "ih",
    "ih1" : "ix",
}

file = open ("cmudict.0.7a", "r")

for line in file:
    tokens = line.lower().split()
    word = re.sub ("\([0-9]+\)", "", tokens[0])
    result = "("
    result += "\"%s\" nil (" % (word)
    for phone in tokens[1:]:
	if phone in replacement.keys():
	    new_phone = replacement[phone]
	else:
	    new_phone = phone.strip("012")
	result += " %s" % (new_phone)
    result += "))"
    print result

    