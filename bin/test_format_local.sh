if [ ! -f ".clang-format" ]; then
    echo ".clang-format file not found!"
    exit 1
fi

if [ "$(uname)" == "Darwin" ]; then
    # Do something under Mac OS X platform
    CLANG="clang-format"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Do something under GNU/Linux platform
    CLANG="clang-format-3.7"
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    # Do something under Windows NT platform
    CLANG="clang-format.exe"
fi

FILES=`git ls-files | grep -E "\.(cpp|h|hpp)$" | grep -Ev "symengine/utilities" | grep -Ev "cmake/"`

for FILE in $FILES; do
    echo Formatting $FILE
    $CLANG -i $FILE
done
