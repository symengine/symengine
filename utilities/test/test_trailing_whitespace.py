from os import walk, sep, pardir
from os.path import split, join, abspath, exists, isfile , basename
from glob import glob
import re
import random
import ast
import inspect
 
_failed_expectations = []
 
def _log_failure(msg=None):
    (filename, line, funcname, contextlist) =  inspect.stack()[2][1:5]
    filename = basename(filename)
    context = contextlist[0]
    _failed_expectations.append('%s\n' % 
        ((('%s' % msg) if msg else '')))
 
def _report_failures():
    global _failed_expectations
    if _failed_expectations:
        (filename, line, funcname) =  inspect.stack()[1][1:4]
        report = [
            'Failed Expectations:%s\n' % len(_failed_expectations)]
        for i,failure in enumerate(_failed_expectations, start=1):
            report.append('%d: %s' % (i, failure))
        _failed_expectations = []
    return ('\n'.join(report))

SYMENGINE_PATH = abspath(join(split(__file__)[0], pardir, pardir))  # go to symengine/
assert exists(SYMENGINE_PATH)

TOP_PATH = abspath(join(SYMENGINE_PATH, pardir))
BIN_PATH = join(TOP_PATH, "bin")

# Trailing whitespace location
message_space = "%s, line %s."

def check_directory_tree(base_path, file_check, exclusions=set(), pattern="*.cpp"):
    """
    Checks all files in the directory tree (with base_path as starting point)
    with the file_check function provided, skipping files that contain
    any of the strings in the set provided by exclusions.
    """
    if not base_path:
        return
    for root, dirs, files in walk(base_path):
        check_files(glob(join(root, pattern)), file_check, exclusions)

def check_files(files, file_check, exclusions=set(), pattern=None):
    """
    Checks all files with the file_check function provided, skipping files
    that contain any of the strings in the set provided by exclusions.
    """
    if not files:
        return
    for fname in files:
        if not exists(fname) or not isfile(fname):
            continue
        if any(ex in fname for ex in exclusions):
            continue
        if pattern is None or re.match(pattern, fname):
            file_check(fname)
            
    """
    This test tests files and checks that:
      o no lines contains a trailing whitespace
    """

def test(fname):

	with open(fname, "rt") as test_file:
		test_this_file(fname, test_file)

	with open(fname, "rt") as test_file:
		source = test_file.read()
	
def test_this_file(fname, test_file):
	line = None  # to flag the case where there were no lines in file
	tests = 0
	test_set = set()
	for idx, line in enumerate(test_file):
		if line.endswith(" \n") or line.endswith("\t\n"):
			_log_failure( message_space % (fname, idx + 1))

exclude = set()

check_directory_tree(BIN_PATH, test, set(["~",".sh"]), "*")
check_directory_tree(SYMENGINE_PATH, test, set(["/build/","/doc/","/cmake/","/utilities"]))
check_directory_tree(SYMENGINE_PATH, test, set(["/build/","/doc/","/cmake/","/utilities"]), "*.h")
print _report_failures()
