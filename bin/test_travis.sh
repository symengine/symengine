#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

if [[ "${WITH_SANITIZE}" != "" ]]; then
        export CXXFLAGS="-fsanitize=${WITH_SANITIZE}"
        if [[ "${WITH_SANITIZE}" == "address" ]]; then
            export ASAN_OPTIONS=symbolize=1,detect_leaks=1,external_symbolizer_path=/usr/lib/llvm-12/bin/llvm-symbolizer
        elif [[ "${WITH_SANITIZE}" == "undefined" ]]; then
            export UBSAN_OPTIONS=print_stacktrace=1,halt_on_error=1,external_symbolizer_path=/usr/lib/llvm-12/bin/llvm-symbolizer
        elif [[ "${WITH_SANITIZE}" == "memory" ]]; then
            # for reference: https://github.com/google/sanitizers/wiki/MemorySanitizerLibcxxHowTo#instrumented-libc
            echo "=== Building libc++ instrumented with memory-sanitizer (msan) for detecting use of uninitialized variables"
            LLVM_ORG_VER=12.0.1  # should match llvm-X-dev package.
            export CC=clang-12
            export CXX=clang++-12
            curl -Ls https://github.com/llvm/llvm-project/archive/llvmorg-${LLVM_ORG_VER}.tar.gz | tar xz -C /tmp
            ( \
              set -xe; \
              mkdir /tmp/build_libcxx; \
              cd /tmp/build_libcxx; \
              cmake \
                  -DCMAKE_BUILD_TYPE=Debug \
                  -DLLVM_USE_SANITIZER=MemoryWithOrigins \
                  -DLLVM_CONFIG_PATH=/usr/bin/llvm-config-12 \
                  -DCMAKE_INSTALL_PREFIX=/opt/libcxx-12-msan \
                  /tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxx; \
              echo "Current dir:"; \
              pwd; \
              cmake --build . ;\
              cmake --build . --target install
            )
            echo "=== Building libc++abi instrumented with memory-sanitizer"
            ( \
              set -xe;
              mkdir /tmp/build_libcxxabi; \
              cd /tmp/build_libcxxabi; \
              cmake \
                  -DCMAKE_BUILD_TYPE=Debug \
                  -DCMAKE_MODULE_PATH=/tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxx/cmake/Modules \
                  -DLLVM_USE_SANITIZER=MemoryWithOrigins  \
                  -DLLVM_CONFIG_PATH=/usr/bin/llvm-config-12 \
                  -DCMAKE_INSTALL_PREFIX=/opt/libcxx-12-msan  \
                  -DLIBCXXABI_LIBCXX_INCLUDES=/opt/libcxx-12-msan/include/c++/v1 \
                  -DLIBCXXABI_LIBCXX_PATH=/tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxx \
                  /tmp/llvm-project-llvmorg-${LLVM_ORG_VER}/libcxxabi; \
              echo "Current dir:"; \
              pwd; \
              cmake --build . ; \
              cmake --build . --target install
            )
            if [ ! -e /opt/libcxx-12-msan/lib/libc++abi.so ]; then >&2 echo "Failed to build libcxx++abi?"; exit 1; fi
            export MSAN_OPTIONS=print_stacktrace=1,halt_on_error=1,external_symbolizer_path=/usr/lib/llvm-12/bin/llvm-symbolizer
            export CXXFLAGS="$CXXFLAGS -fsanitize-memory-track-origins=2 -stdlib=libc++ -I/opt/libcxx-12-msan/include -I/opt/libcxx-12-msan/include/c++/v1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -O1 -glldb -DHAVE_GCC_ABI_DEMANGLE=no"
            export LDFLAGS="-fsanitize=memory -fsanitize-memory-track-origins=2 $LDFLAGS -Wl,-rpath,/opt/libcxx-12-msan/lib -L/opt/libcxx-12-msan/lib -lc++abi"
        else
            2>&1 echo "Unknown sanitize option: ${WITH_SANITIZE}"
            exit 1
        fi
elif [[ "${CC}" == *"clang"* ]] && [[ "$(uname)" == "Linux" ]]; then
    if [[ "${BUILD_TYPE}" == "Debug" ]]; then
        export CXXFLAGS="$CXXFLAGS -ftrapv"
    fi
elif [[ "$(uname)" == "Linux" ]]; then
    export CXXFLAGS="$CXXFLAGS -Werror"
    if [[ "${USE_GLIBCXX_DEBUG}" == "yes" ]]; then
        export CXXFLAGS="$CXXFLAGS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC"
    fi
fi

echo "=== Generating cmake command from environtment variables"

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
if [[ "${WITH_COTIRE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_COTIRE=${WITH_COTIRE}"
fi
if [[ "${WITH_LLVM}" != "" ]] ; then
    cmake_line="$cmake_line -DWITH_LLVM:BOOL=ON -DLLVM_DIR=${LLVM_DIR}"
fi
if [[ "${BUILD_DOXYGEN}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_DOXYGEN=${BUILD_DOXYGEN}"
fi
if [[ "${CC}" == *"gcc"* ]] && [[ "$(uname)" == "Darwin" ]]; then
    cmake_line="$cmake_line -DBUILD_FOR_DISTRIBUTION=yes"
fi

echo "=== Generating build scripts for SymEngine using cmake"
echo "CMAKE_GENERATOR = ${CMAKE_GENERATOR}"
echo "Current directory:"
export BUILD_DIR=`pwd`
pwd
echo "Running cmake:"
cmake $cmake_line ${SOURCE_DIR}


echo "=== Running build scripts for SymEngine"
pwd
echo "Running make" $MAKEFLAGS ":"
make VERBOSE=1

echo "Running make install:"
make install

if [[ "${TEST_CPP}" == "no" ]]; then
    return 0;
fi

echo "=== Running tests in build directory:"
# C++
ctest --output-on-failure

if [[ "${WITH_COVERAGE}" == "yes" ]]; then
    echo "=== Collecting coverage data"
    curl --connect-timeout 10 --retry 5 -L https://codecov.io/bash -o codecov.sh
    bash codecov.sh -x $GCOV_EXECUTABLE 2>&1 | grep -v "has arcs to entry block" | grep -v "has arcs from exit block"
    return 0;
fi

if [[ "${WITH_SANITIZE}" != "" ]]; then
    # currently compile_flags and link_flags below won't pick up -fsanitize=...
    return 0;
fi

echo "=== Testing the installed SymEngine library simulating use by 3rd party lib"
cd $SOURCE_DIR/benchmarks

SymEngine_DIR="${our_install_dir}/lib/cmake/symengine"
if [[ "${MSYSTEM}" != "" ]]; then
  SymEngine_DIR="${our_install_dir}/CMake"
fi
compile_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$SymEngine_DIR -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=COMPILE`
link_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$SymEngine_DIR  -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=LINK`

${CXX} -std=c++0x $compile_flags expand1.cpp -o expand1 $link_flags
export LD_LIBRARY_PATH=$our_install_dir/lib:$LD_LIBRARY_PATH
./expand1

echo "Checking whether all header files are installed:"
python $SOURCE_DIR/bin/test_make_install.py $our_install_dir/include/symengine/ $SOURCE_DIR/symengine
