#!/usr/bin/env bash

if [[ "${TRAVIS_OS_NAME}" == "linux" ]] && \
   [[ "${CC}" == "gcc" ]] && [[ "${TEST_CLANG_FORMAT}" == "yes" ]]; then

    RETURN=0
    CLANG="clang-format-3.7"

    if [ ! -f ".clang-format" ]; then
        echo ".clang-format file not found!"
        exit 1
    fi

    FILES=`git diff master --name-only | grep -E "\.(cpp|h)$"`

    for FILE in $FILES; do

        $CLANG $FILE | cmp  $FILE >/dev/null

        if [ $? -ne 0 ]; then
            echo "[!] INCORRECT FORMATTING! $FILE" >&2
            RETURN=1
        fi

    done

    exit $RETURN
fi

exit 0
