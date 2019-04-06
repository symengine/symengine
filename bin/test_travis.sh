#!/usr/bin/env bash

fold_start() {
  echo -e "travis_fold:start:$1\033[33;1m$2\033[0m"
}

fold_end() {
  echo -e "\ntravis_fold:end:$1\r"
}

# Exit on error
set -e
# Echo each command
set -x

if [[ "${WITH_SANITIZE}" != "" ]]; then
	export CXXFLAGS="-fsanitize=${WITH_SANITIZE}"
	if [[ "${WITH_SANITIZE}" == "address" ]]; then
	    export ASAN_OPTIONS=symbolize=1,detect_leaks=1,external_symbolizer_path=/usr/lib/llvm-7/bin/llvm-symbolizer
	elif [[ "${WITH_SANITIZE}" == "undefined" ]]; then
	    export UBSAN_OPTIONS=print_stacktrace=1,halt_on_error=1,external_symbolizer_path=/usr/lib/llvm-7/bin/llvm-symbolizer
	elif [[ "${WITH_SANITIZE}" == "memory" ]]; then
	    export MSAN_OPTIONS=abort_on_error=1,external_symbolizer_path=/usr/lib/llvm-7/bin/llvm-symbolizer
            # for reference: https://github.com/google/sanitizers/wiki/MemorySanitizerLibcxxHowTo#instrumented-libc
            fold_start libcxx.1 "Building libc++ instrumented with memory-sanitizer (msan) for detecting use of uninitialized variables"
            LLVM_ORG_VER=7.0.1  # should match llvm-X-dev package.
            export CC=clang-7
            export CXX=clang++-7
            curl -Ls https://github.com/llvm/llvm-project/archive/llvmorg-${LLVM_ORG_VER}.tar.gz | tar xz -C /tmp
            ( \
              set -xe; \
              mkdir /tmp/build_libcxx; \
              cd /tmp/build_libcxx; \
              cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_SANITIZER=Memory -DCMAKE_INSTALL_PREFIX=/opt/libcxx_msan /tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxx; \
              echo "Current dir:"; \
              pwd; \
              cmake --build . ;\
              cmake --build . --target install
            )
            fold_end libcxx.1
            echo "---"
            fold_start libcxxabi.1 "Building libc++abi instrumented with memory-sanitizer"
            ( \
              set -xe;
              mkdir /tmp/build_libcxxabi; \
              cd /tmp/build_libcxxabi; \
              cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_SANITIZER=Memory -DCMAKE_INSTALL_PREFIX=/opt/libcxx_msan /tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxxabi; \
              echo "Current dir:"; \
              pwd; \
              cmake --build . ;\
              cmake --build . --target install
            )
            if [ ! -e /opt/libcxx_msan/lib/libc++abi.so ]; then >&2 echo "Failed to build libcxx++abi?"; exit 1; fi
            fold_end libcxxabi.1
            export CXXFLAGS="$CXXFLAGS -stdlib=libc++ -I/opt/libcxx_msan/include -I/opt/libcxx_msan/include/c++/v1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1 -glldb -DHAVE_GCC_ABI_DEMANGLE=no"
            export LDFLAGS="-fsanitize=memory $LDFLAGS -Wl,-rpath,/opt/libcxx_msan/lib -L/opt/libcxx_msan/lib -lc++abi"
            #export CMAKE_CXX_FLAGS_DEBUG="$CXXFLAGS"
            #unset CXXFLAGS
            #echo "CMAKE_CXX_FLAGS_DEBUG=$CMAKE_CXX_FLAGS_DEBUG"  # debug
	else
	    2>&1 echo "Unknown sanitize option: ${WITH_SANITIZE}"
	    exit 1
	fi
elif [[ "${CC}" == *"clang"* ]] && [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then
    if [[ "${BUILD_TYPE}" == "Debug" ]]; then
        export CXXFLAGS="$CXXFLAGS -ftrapv"
    fi
else
    export CXXFLAGS="$CXXFLAGS -Werror"
    if [[ "${USE_GLIBCXX_DEBUG}" == "yes" ]]; then
        export CXXFLAGS="$CXXFLAGS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC"
    fi
fi

fold_start symengine_cmake_line.1 "Generating cmake command from environtment variables"

# Shippable currently does not clean the directory after previous builds
# (https://github.com/Shippable/support/issues/238), so
# we need to do it ourselves.
git clean -dfx

if [[ "${TEST_IN_TREE}" != "yes" ]]; then
    mkdir build
    cd build
fi
# We build the command line here. If the variable is empty, we skip it,
# otherwise we pass it to cmake.
cmake_line="-DCMAKE_INSTALL_PREFIX=$our_install_dir -DCMAKE_PREFIX_PATH=$our_install_dir"
if [[ "${BUILD_TYPE}" != "" ]]; then
    cmake_line="$cmake_line -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi
if [[ "${WITH_BFD}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_BFD=${WITH_BFD}"
fi
if [[ "${WITH_SYMENGINE_ASSERT}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_ASSERT=${WITH_SYMENGINE_ASSERT}"
fi
if [[ "${WITH_SYMENGINE_RCP}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_RCP=${WITH_SYMENGINE_RCP}"
fi
if [[ "${WITH_SYMENGINE_THREAD_SAFE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_THREAD_SAFE=${WITH_SYMENGINE_THREAD_SAFE}"
fi
if [[ "${WITH_ECM}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_ECM=${WITH_ECM}"
fi
if [[ "${WITH_PRIMESIEVE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PRIMESIEVE=${WITH_PRIMESIEVE}"
fi
if [[ "${WITH_ARB}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_ARB=${WITH_ARB}"
fi
if [[ "${WITH_MPFR}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_MPFR=${WITH_MPFR}"
fi
if [[ "${WITH_MPC}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_MPC=${WITH_MPC}"
fi
if [[ "${WITH_PIRANHA}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PIRANHA=${WITH_PIRANHA}"
fi
if [[ "${WITH_FLINT}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_FLINT=${WITH_FLINT}"
fi
if [[ "${WITH_BENCHMARKS_NONIUS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_BENCHMARKS_NONIUS=${WITH_BENCHMARKS_NONIUS}"
fi
if [[ "${BUILD_SHARED_LIBS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
fi
if [[ "${WITH_RUBY}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_RUBY=${WITH_RUBY}"
fi
if [[ "${TEST_CPP}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_BENCHMARKS=${TEST_CPP} -DBUILD_TESTS=${TEST_CPP}"
fi
if [[ "${BUILD_BENCHMARKS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_BENCHMARKS=${BUILD_BENCHMARKS}"
fi
if [[ "${INTEGER_CLASS}" != "" ]]; then
    cmake_line="$cmake_line -DINTEGER_CLASS=${INTEGER_CLASS}"
fi
if [[ "${WITH_COVERAGE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_COVERAGE=${WITH_COVERAGE}"
fi
if [[ "${WITH_LLVM}" != "" ]] ; then
    cmake_line="$cmake_line -DWITH_LLVM:BOOL=ON -DLLVM_DIR=${LLVM_DIR}"
fi
if [[ "${BUILD_DOXYGEN}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_DOXYGEN=${BUILD_DOXYGEN}"
fi
if [[ "${CC}" == *"gcc"* ]] && [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
    cmake_line="$cmake_line -DBUILD_FOR_DISTRIBUTION=yes"
fi

fold_end symengine_cmake_line.1

fold_start symengine_cmake_config.1 "Generating build scripts for SymEngine using cmake"
echo "Current directory:"
export BUILD_DIR=`pwd`
pwd
echo "Running cmake:"
cmake $cmake_line ${SOURCE_DIR}
fold_end symengine_cmake_config.1


fold_start symengine_cmake_build.1 "Running build scripts for SymEngine"
echo "Current directory:"
pwd
echo "Running make" $MAKEFLAGS ":"
make
fold_end symengine_cmake_build.1

fold_start symengine_cmake_install.1 "Installing SymEngine"
echo "Running make install:"
make install
fold_end symengine_cmake_install.1 "Installing SymEngine"

if [[ "${TEST_CPP}" == "no" ]]; then
    exit 0;
fi

fold_start symengine_cmake_test.1 "Running the test suite of SymEngine"
echo "Running tests in build directory:"
# C++
if [[ "${WITH_SANITIZE}" == "memory" ]]; then
    ctest --output-on-failure --exclude-regex "(test_bipartite)|(test_hopcroft_karp)"  # TODO: debug the reason for failure
else
    ctest --output-on-failure
fi
fold_end symengine_cmake_test.1 "Running the test suite of SymEngine"

if [[ "${WITH_COVERAGE}" == "yes" ]]; then
    fold_start symengine_coverage.1 "Collecting coverage data"
    curl -L https://codecov.io/bash -o codecov.sh
    bash codecov.sh -x $GCOV_EXECUTABLE 2>&1 | grep -v "has arcs to entry block" | grep -v "has arcs from exit block"
    fold_end symengine_coverage.1
    exit 0;
fi

if [[ "${WITH_SANITIZE}" != "" ]]; then
    # currently compile_flags and link_flags below won't pick up -fsanitize=...
    exit 0;
fi

fold_start symengine_standalone_test.1 "Testing the installed SymEngine library simulating use by 3rd party lib"
echo "Running tests using installed SymEngine:"

cd $SOURCE_DIR/benchmarks

compile_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$our_install_dir/lib/cmake/symengine -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=COMPILE`
link_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$our_install_dir/lib/cmake/symengine  -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=LINK`

${CXX} -std=c++0x $compile_flags expand1.cpp $link_flags
export LD_LIBRARY_PATH=$our_install_dir/lib:$LD_LIBRARY_PATH
./a.out
fold_end symengine_standalone_test.1


echo "Checking whether all header files are installed:"
python $SOURCE_DIR/bin/test_make_install.py $our_install_dir/include/symengine/ $SOURCE_DIR/symengine
