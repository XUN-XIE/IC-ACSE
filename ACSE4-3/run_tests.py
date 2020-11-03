import os
import os.path
import glob
import subprocess
import sys

import tests.python_tests

DIR = "tests/bin/*"

files = glob.glob(DIR)

glob_fail = False
for file in files:
    if sys.platform == "win32" and file.split(".")[-1] != "exe":
         continue
    if not os.path.isdir(file) and "test" in file:  # skip directories and unwanted files such as core dumps
        output = subprocess.run([file], stdout=subprocess.PIPE, universal_newlines=True)
        test_failed = False
        if output.returncode:
            test_failed = True
        if "fail" in output.stdout:
            test_failed = True
        if test_failed:
            print("Test %s failed" % file)
            glob_fail = True
        else:
            print("Test %s passed" % file)

for name, attr in tests.python_tests.__dict__.items():
    if 'test' in name:
        try:
            attr()
            print("Test %s passed" %name)
        except Exception:
            print("Test %s failed" %name)
            glob_fail = True
            
if glob_fail:
    sys.exit(1)
