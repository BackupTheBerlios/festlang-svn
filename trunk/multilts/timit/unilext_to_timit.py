#!/usr/bin/python

import re

# cmudict to timit

replacement = {
"@" :    "ah",
"@@r" :  "er",
"@r" :   "axr",
"a" :    "ae",
"aa" :   "aa",
"aer" :  "ay", ####### aer r -> ay axr
"ai" :   "ay",
"ar" :   "aa",
"b" :    "b",
"ch" :   "ch",
"d" :    "d",
"dh" :   "dh",
"e" :    "eh",
"eh" :   "eh",
"ei" :   "ey",
"eir" :  "eh r",
"f" :    "f",
"g" :    "g",
"h" :    "hh",
"hw" :   "w",
"i" :    "ih",
"ii" :   "ix",
"ir" :   "ih",
"jh" :   "jh",
"k" :    "k",
"l" :    "l",
"l=" :   "el",
"lw" :   "l",
"m" :    "m",
"m=" :   "em",
"n" :    "n",
"n=" :   "en",
"ng" :   "ng",
"oi" :   "oy",
"oir" :  "", ########## not significant
"oo" :   "ao",
"or" :   "ao",
"ou" :   "ow",
"ow" :   "aw",
"owr" :  "aw", ########## owr r -> aw axr
"p" :    "p",
"r" :    "r",
"s" :    "s",
"sh" :   "sh",
"t" :    "t",
"t^" :   "dx",
"th" :   "th",
"u" :    "uh",
"uh" :   "ah",
"ur" :   "uh",
"uu" :   "ux",
"v" :    "v",
"w" :    "w",
"y" :    "y",
"z" :    "z",
"zh" :   "zh",
}

file = open ("a.a", "r")

for line in file:
    tokens = line.lower().split()
    word = tokens[0]
    result = "("
    result += "\"%s\" nil (" % (word)
    for i in range(1,len(tokens)):
	phone = tokens[i]
	if phone == "oir":
	    continue;
	if phone == "r" and (tokens[i-1] == "aer" or tokens[i-1] == "owr"):
	    result += " axr"
	    continue;
	    
	if phone in replacement.keys():
	    new_phone = replacement[phone]
	else:
	    new_phone = phone
	result += " %s" % (new_phone)
    result += "))"
    print result

    