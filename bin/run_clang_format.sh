#!/usr/bin/env bash

RETURN=0

clang-format --version

if [ ! -f ".clang-format" ]; then
    echo ".clang-format file not found!"
    exit 1
fi

FILES=`git ls-files | grep -E "\.(cpp|h|hpp|c)$" | grep -Ev "symengine/utilities" | grep -Ev "cmake/"`

for FILE in $FILES; do
    echo "Processing: $FILE"

    clang-format $FILE | cmp  $FILE >/dev/null

    if [ $? -ne 0 ]; then
        echo "[!] INCORRECT FORMATTING! $FILE" >&2
        clang-format -i $FILE
        RETURN=1
    fi

done

if [ $RETURN -ne 0 ]; then
    RED='\033[0;31m'
    echo -e "\\n${RED}FORMATTING TEST FAILED\\n"
    echo "Apply the following diff for correct formatting"
    echo "###########################################################################"
    git diff | cat
    echo "###########################################################################"
else
    GREEN='\033[0;32m'
    echo -e "\\n${GREEN}FORMATTING TEST PASSED\\n"
fi
exit $RETURN
