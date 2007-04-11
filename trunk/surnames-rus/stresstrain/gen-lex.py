#!/usr/bin/python
# -*- coding: utf-8 -*-

vowels = ("�", "�", "�", "�", "�", "�", "�", "�", "�", "�")

def ends_with (string, postfix):
    return (string + "#").find (postfix + "#") >= 0

def stress_syllable (string):

    count = 0
    i = 0
    while i < len(string):
	if string[i] == '\'':
	    break;
	if vowels.__contains__(string[i:i+1]):
	    count = count + 1
	i = i + 1
    return count + 1

def get_pos (input_word, i):

    if ends_with (input_word, "���") or ends_with (input_word, "���"):
	    return "sname"

    if i < 2300:
	    return "name"

    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-in"
    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-in"
    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-yan"
    if ends_with (input_word, "��") or  ends_with (input_word, "��"):
	    return "surname-ih"
    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-ev"
    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-ov"
    if ends_with (input_word, "��") or ends_with (input_word, "���"):
	    return "surname-ov"
    if ends_with (input_word, "����") or ends_with (input_word, "����"):
	    return "surname-ski"
    if ends_with (input_word, "����") or ends_with (input_word, "����"):
	    return "surname-ski"
    if ends_with (input_word, "��"):
	    return "surname-ko"
    if ends_with (input_word, "��") or ends_with (input_word, "��"):
	    return "surname-uk"
    if ends_with (input_word, "��"):
	    return "surname-ik"

    return "name"

def main():

    file = open ("test.data", "r")
    lines = file.readlines ()


    j = 0
    for line in lines:

	test_word = line.strip()
	input_word = test_word.replace ("'","")
	pos = get_pos (input_word, j)

	j = j + 1;
	print ("(\"%s\" %s (%d))" % (input_word, pos, stress_syllable (test_word)))

main()

