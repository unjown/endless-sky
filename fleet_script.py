import re, os, sys

def distance_formula(value, power):
    return(value + pow(value, power))
    
nargs = len(sys.argv)

file_in = sys.argv[1]
file_read = open(file_in, 'r')
output = open(file_in + ".out", 'w')
full = file_read.readlines()
fleet_ratio_1 = float(sys.argv[2])
fleet_ratio_2 = float(sys.argv[3])
for line in full:
    if(line.startswith('\tfleet "Small Republic"')):
        fleet_freq = float(line[24:])
        output.write('\tfleet "Small Republic" ' + str(int(round(fleet_freq * fleet_ratio_1, 1))) + '\n\tfleet "Republic Police" ' + str(int(round(fleet_freq * fleet_ratio_2, 1))) + "\n")
    else:
        output.write(line)
file_read.close