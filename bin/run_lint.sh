# This is a temporary fix till the python script is prepared (#764).
# To be used locally while development.
export SOURCE_DIR=`pwd`

# check trailing whitespace:
if !  egrep " $" -nr --include=\*.{cpp,h,inc}  --exclude-dir=*teuchos* $SOURCE_DIR ; then
    echo No trailing whitespace;
else
    echo No trailing whitespace check failed;
    exit -1;
fi

# check space after comma
if !  egrep "^([^(//)])*((,[^\ \n])|(,\ \ +))" -nr --include=\*.{cpp,h,inc}  --exclude-dir=*teuchos* $SOURCE_DIR ; then
    echo Space after comma check passed;
else
    echo Space after comma check failed;
    exit -1;
fi

# check space between `)` and `{`
if !  egrep "(\)\{)|(\)\ \ +\{)" -nr --include=\*.{cpp,h,inc}  --exclude-dir=*teuchos* $SOURCE_DIR ; then
    echo \"\)-space-\{\" check passed;
else
    echo \"\)-space-\{\" check failed;
    exit -1;
fi

# check space after `if`
if ! egrep "((^if)|(\ if))((\()|(\ \ +\())" -nr --include=\*.{cpp,h,inc}  --exclude-dir=*teuchos* $SOURCE_DIR ; then
    echo \"if-space\" check passed;
else
    echo \"if-space\" check failed;
    exit -1;
fi

# check space around `=` and `==`
if ! egrep "^[^(//)]*[^\ (operator)]=[=][^\ ]" -nr --include=\*.{cpp,h,inc}  --exclude-dir=*teuchos* $SOURCE_DIR ; then
    echo Space around = and == check passed;
else
    echo Space around = and == check failed;
    exit -1;
fi

