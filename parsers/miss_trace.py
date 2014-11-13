#!/ufs/cluster/yao/bin/python

import re, os
import string
import sys
import array
import random
import shutil
from os import walk

def genTrace(input_filename):
    print "Parsing " + input_filename
    # output filename
    output_filename = input_filename.split('.')[0] + ".trc"
    # open input file
    inputfile = open(input_filename, "r")
    # open output file
    outputfile = open(output_filename, "w")
    # read lines from input file
    linePattern = "Miss count"
    lineCount = 0
    for line in inputfile:
        searchResult = line.find(linePattern)
        if searchResult != -1:
            missCount = line.split(' ')[-1]
            try:
                int(missCount)
                lineCount += 1
                outputfile.write("%s %s" % (lineCount, missCount))
            except ValueError:          
                print missCount
                
    inputfile.close()
    outputfile.close()
    # # create output folder if not exist
    # dn = os.path.dirname(input_filename)
    # print dn
    # output_folder = dn + "/traces"
    # print output_folder
    # if not os.path.exists(output_folder):
    #     os.makedirs(output_folder)
    # # move the output file to output folder
    # shutil.move(output_filename, output_folder)

if len(sys.argv) != 2:
    sys.exit("Must specify folder")

folder = sys.argv[1]
# get file names in the folder
f = []
for (dirpath, dirnames, filenames) in walk(folder):
    f.extend(filenames)
    break
# generate traces for all the files
for file in f:
    genTrace(folder+file)