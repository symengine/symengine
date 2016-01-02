from os import walk, pardir
from os.path import split, join, abspath, exists

### CHANGE THIS IF THIS FILE IS EVER MOVED
### RIGHT NOW IT'S IN symengine/symengine/utilities/tests/
SYMENGINE_PATH = abspath(join(split(__file__)[0], pardir, pardir, pardir))
assert exists(SYMENGINE_PATH)

class Check:
    """
    A class which defines a particular 'check'.

    Arguments
    =========
    checker_fx          : [function] takes in absolute filepath as argument and does the checking
    root_dirs           : [list] directories on which the check will run recursively
    included_filetypes  : [optional] [list] patterns, the file will be matched against
    excluded_names      : [optional] [list] file won't be checked, if it's abs path is `in` excluded_names
    """

    def __init__(self, checker_fx, root_dirs, included_filetypes=["."], excluded_names=[]):
        self.fx = checker_fx
        self.root_dirs = root_dirs
        self.included_filetypes = included_filetypes
        self.excluded_names = excluded_names

# Construct your check classes here
all_checks = []
errors = False

# trailing whitespace check
def trailing_whitespace_function(file_path):
    global errors
    with open(file_path) as file_ptr:
        for line_num, line in enumerate(file_ptr):
            if line.endswith(" \n") or line.endswith("\t\n"):
                print "Trailing whitespace :", file_path, str(line_num + 1)
                errors = True

trailing_whitespace_check = Check(trailing_whitespace_function, [SYMENGINE_PATH])
trailing_whitespace_check.excluded_names = ["/build/", "/doc/"]
trailing_whitespace_check.included_filetypes = [".cpp", ".h", ".py", ".sh"]
all_checks.append(trailing_whitespace_check)

# add another check below

# Main
for check in all_checks:
    for base_dir in check.root_dirs:
        for root, dirs, file_names in walk(base_dir):
            for file_name in file_names:
                file_path = join(root, file_name)
                if any(excluded_name in file_path for excluded_name in check.excluded_names):
                    continue
                if any(file_path.endswith(file_type) for file_type in check.included_filetypes):
                    check.fx(file_path)

if errors:
    print "Errors Listed above!"
    exit(1)
exit(0)
