#!/usr/bin/python

import sys

resident_ave = 0
resident_max = 0
virtual_ave = 0
virtual_max = 0

nlines = 0

for line in sys.stdin:
    oline = line.strip(" ").replace("  ", " ")
    while len(oline) != len(line):
        line = oline
        oline = line.replace("  ", " ")

    tokens = line.split(" ")

    if len(tokens) < 6:
        continue


    if tokens[4][-1] == 'm':
        virtual = float(tokens[4][:-1]) * 1024 * 1024

    elif tokens[4][-1] == 'g':
        virtual = float(tokens[4][:-1]) * 1024 * 1024 * 1024

    else:
        virtual = float(tokens[4])

    if tokens[5][-1] == 'm':
        resident = float(tokens[5][:-1]) * 1024 * 1024

    elif tokens[5][-1] == 'g':
        resident = float(tokens[5][:-1]) * 1024 * 1024 * 1024

    else:
        resident = float(tokens[5])


    virtual_ave += virtual
    virtual_max = max(virtual_max, virtual)

    resident_ave += resident
    resident_max = max(resident_max, resident)

    nlines+=1

resident_ave /= nlines
virtual_ave /= nlines

resident_ave /= 1024*1024
resident_max /= 1024*1024
virtual_ave /= 1024*1024
virtual_max /= 1024*1024

print "max virtual: ", virtual_max, "m"
print "ave virtual: ", virtual_ave, "m"
print "max resident: ", resident_max, "m"
print "ave resident: ", resident_ave, "m"

