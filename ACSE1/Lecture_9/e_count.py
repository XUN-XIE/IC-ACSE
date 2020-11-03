import sys

e_count = 0
with open(sys.argv[1],'r') as infile:
    for line in infile.readlines():
        e_count += line.count('e')
print("There were %d letter e's"%e_count)