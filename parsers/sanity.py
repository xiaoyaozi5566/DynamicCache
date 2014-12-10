import re, os
import string
import sys
import array
import random
import shutil
from subprocess import call
from os import walk

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    
def sanity_check(ref_filename, check_filename):
    print "Checking " + check_filename + " against " + ref_filename
    ref_file = open(ref_filename, "r")
    check_file = open(check_filename, "r")
    
    failFlag = False
    
    for line0 in ref_file:
        searchResult0 = line0.find("system.cpu")
        if searchResult0 != -1:
            stopFlag = False
            searchResult1 = -1
            while searchResult1 == -1:
                try:
                    line1 = check_file.next()
                    searchResult1 = line1.find("system.cpu1")
                except StopIteration:
                    stopFlag = True
                    break
            
            if stopFlag:
                print bcolors.OKGREEN + "PASS" + bcolors.ENDC
                break
                        
            line00 = line0[searchResult0+11:]
            line11 = line1[searchResult1+12:]
            if line00 != line11:
                failFlag = True
                print line0 + line1
                print bcolors.FAIL + "FAIL" + bcolors.ENDC
                break
    
    if not failFlag:
        print bcolors.OKGREEN + "PASS" + bcolors.ENDC

if len(sys.argv) != 3:
    sys.exit("Usage: sanity.py file1 file2")

ref_filename   = sys.argv[1]
check_filename = sys.argv[2]

sanity_check(ref_filename, check_filename)
