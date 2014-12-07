import re, os
import string
import sys
import array
import random
import shutil
from subprocess import call
from os import walk

def sanity_check(ref_filename, check_filename):
    print "Checking " + check_filename + " against " + ref_filename
    ref_file = open(ref_filename, "r")
    ref_out  = open(ref_filename+'.tmp', "w")
    check_file = open(check_filename, "r")
    check_out  = open(check_filename+'.tmp', "w")
    
    for line in ref_file:
        searchResult = line.find("system.cpu")
        if searchResult != -1:
            ref_out.write("%s" % line[searchResult+11:])
    
    for line in check_file:
        searchResult = line.find("system.cpu0")
        if searchResult != -1:
            check_out.write("%s" % line[searchResult+12:])
    
    # check difference
    call(["diff", ref_filename+'.tmp', check_filename+'.tmp'])
    # remove tempory files
    ref_out.close()
    check_out.close()
    call(["rm", "-f", ref_filename+'.tmp', check_filename+'.tmp'])
    # close file
    ref_file.close()
    check_file.close()
    

if len(sys.argv) != 3:
    sys.exit("Usage: sanity.py file1 file2")

ref_filename   = sys.argv[1]
check_filename = sys.argv[2]

sanity_check(ref_filename, check_filename)
