# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/akash/symengine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/akash/symengine

# Include any dependencies generated for this target.
include symengine/tests/basic/CMakeFiles/test_polynomial.dir/depend.make

# Include the progress variables for this target.
include symengine/tests/basic/CMakeFiles/test_polynomial.dir/progress.make

# Include the compile flags for this target's objects.
include symengine/tests/basic/CMakeFiles/test_polynomial.dir/flags.make

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o: symengine/tests/basic/CMakeFiles/test_polynomial.dir/flags.make
symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o: symengine/tests/basic/test_polynomial.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/akash/symengine/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o"
	cd /home/akash/symengine/symengine/tests/basic && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o -c /home/akash/symengine/symengine/tests/basic/test_polynomial.cpp

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_polynomial.dir/test_polynomial.cpp.i"
	cd /home/akash/symengine/symengine/tests/basic && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/akash/symengine/symengine/tests/basic/test_polynomial.cpp > CMakeFiles/test_polynomial.dir/test_polynomial.cpp.i

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_polynomial.dir/test_polynomial.cpp.s"
	cd /home/akash/symengine/symengine/tests/basic && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/akash/symengine/symengine/tests/basic/test_polynomial.cpp -o CMakeFiles/test_polynomial.dir/test_polynomial.cpp.s

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.requires:
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.requires

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.provides: symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.requires
	$(MAKE) -f symengine/tests/basic/CMakeFiles/test_polynomial.dir/build.make symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.provides.build
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.provides

symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.provides.build: symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o

# Object files for target test_polynomial
test_polynomial_OBJECTS = \
"CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o"

# External object files for target test_polynomial
test_polynomial_EXTERNAL_OBJECTS =

symengine/tests/basic/test_polynomial: symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o
symengine/tests/basic/test_polynomial: symengine/tests/basic/CMakeFiles/test_polynomial.dir/build.make
symengine/tests/basic/test_polynomial: symengine/libsymengine.a
symengine/tests/basic/test_polynomial: symengine/utilities/catch/libcatch.a
symengine/tests/basic/test_polynomial: symengine/libsymengine.a
symengine/tests/basic/test_polynomial: symengine/utilities/teuchos/libteuchos.a
symengine/tests/basic/test_polynomial: /usr/lib/x86_64-linux-gnu/libgmpxx.so
symengine/tests/basic/test_polynomial: /usr/lib/x86_64-linux-gnu/libgmp.so
symengine/tests/basic/test_polynomial: symengine/tests/basic/CMakeFiles/test_polynomial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_polynomial"
	cd /home/akash/symengine/symengine/tests/basic && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_polynomial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
symengine/tests/basic/CMakeFiles/test_polynomial.dir/build: symengine/tests/basic/test_polynomial
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/build

symengine/tests/basic/CMakeFiles/test_polynomial.dir/requires: symengine/tests/basic/CMakeFiles/test_polynomial.dir/test_polynomial.cpp.o.requires
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/requires

symengine/tests/basic/CMakeFiles/test_polynomial.dir/clean:
	cd /home/akash/symengine/symengine/tests/basic && $(CMAKE_COMMAND) -P CMakeFiles/test_polynomial.dir/cmake_clean.cmake
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/clean

symengine/tests/basic/CMakeFiles/test_polynomial.dir/depend:
	cd /home/akash/symengine && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/akash/symengine /home/akash/symengine/symengine/tests/basic /home/akash/symengine /home/akash/symengine/symengine/tests/basic /home/akash/symengine/symengine/tests/basic/CMakeFiles/test_polynomial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : symengine/tests/basic/CMakeFiles/test_polynomial.dir/depend

