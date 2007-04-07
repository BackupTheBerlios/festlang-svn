#!/usr/bin/python
# -*- coding: utf-8 -*-

vowels = ("а", "у", "о", "е", "ю", "я", "ё", "ы", "э", "и")

def ends_with (string, postfix):
    return (string + "#").find (postfix + "#") >= 0

def stress_syllable (string):

    count = 0
    i = 0
    while i < len(string):
	if string[i] == '\'':
	    break;
	if vowels.__contains__(string[i:i+2]):
	    count = count + 1
	i = i + 2
    return count + 1

def get_pos (input_word):

    if ends_with (input_word, "вич") or ends_with (input_word, "вна"):
	    return "sname"

    if ends_with (input_word, "ин") or ends_with (input_word, "ина"):
	    return "surname-in"
    if ends_with (input_word, "ын") or ends_with (input_word, "ына"):
	    return "surname-in"
    if ends_with (input_word, "ян") or ends_with (input_word, "янц"):
	    return "surname-yan"
    if ends_with (input_word, "ых") or  ends_with (input_word, "их"):
	    return "surname-ih"
    if ends_with (input_word, "ев") or ends_with (input_word, "ева"):
	    return "surname-ovev"
    if ends_with (input_word, "ёв") or ends_with (input_word, "ёва"):
	    return "surname-ovev"
    if ends_with (input_word, "ов") or ends_with (input_word, "ова"):
	    return "surname-ovev"
    if ends_with (input_word, "ский") or ends_with (input_word, "ская"):
	    return "surname-ski"
    if ends_with (input_word, "цкий") or ends_with (input_word, "цкая"):
	    return "surname-ski"
    if ends_with (input_word, "ко"):
	    return "surname-ko"
    if ends_with (input_word, "чук") or ends_with (input_word, "юк"):
	    return "surname-uk"

    return "name"

def main():

    file = open ("test.data", "r")
    lines = file.readlines ()


    for line in lines:

	test_word = line.strip()
	input_word = test_word.replace ("'","")
	pos = get_pos (input_word)

	print ("(\"%s\" %s (%d))" % (input_word, pos, stress_syllable (test_word)))

main()

