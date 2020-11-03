import argparse
import os
import sys
from contextlib import redirect_stdout

import pytest

from . import  BASE_PATH
from .timing import process_results


parser = argparse.ArgumentParser()

parser.add_argument("-v", "--verbose", action="store_true")
parser.add_argument("-o", "--outfile", dest="outfile", default='')
parser.add_argument("-c", "--configfile", dest="configfile", default='')

args = parser.parse_args()
        
DATA_PATH = BASE_PATH+os.sep+"results.xml"
LOG_PATH = BASE_PATH+os.sep+"pytest.log"

try:
    os.remove(DATA_PATH)
except FileNotFoundError:
    pass

try:
    os.remove(LOG_PATH)
except FileNotFoundError:
    pass

if args.configfile:
    os.environ["SCORE_CONFIG_FILE"]=args.configfile

with  open(LOG_PATH, 'w') as log_path:
    if args.verbose:
        stdout = sys.__stdout__
    else:
        stdout = log_path
    with redirect_stdout(stdout):
        pytest.main([BASE_PATH+os.sep+"test.py",
                     "-c%s"%BASE_PATH+os.sep+"pytest.ini", 
                     "--junit-xml=%s"%DATA_PATH])
        

process_results(DATA_PATH, args.outfile)
