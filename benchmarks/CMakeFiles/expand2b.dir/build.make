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
include benchmarks/CMakeFiles/expand2b.dir/depend.make

# Include the progress variables for this target.
include benchmarks/CMakeFiles/expand2b.dir/progress.make

# Include the compile flags for this target's objects.
include benchmarks/CMakeFiles/expand2b.dir/flags.make

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o: benchmarks/CMakeFiles/expand2b.dir/flags.make
benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o: benchmarks/expand2b.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/akash/symengine/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/expand2b.dir/expand2b.cpp.o -c /home/akash/symengine/benchmarks/expand2b.cpp

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/expand2b.dir/expand2b.cpp.i"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/akash/symengine/benchmarks/expand2b.cpp > CMakeFiles/expand2b.dir/expand2b.cpp.i

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/expand2b.dir/expand2b.cpp.s"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/akash/symengine/benchmarks/expand2b.cpp -o CMakeFiles/expand2b.dir/expand2b.cpp.s

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.requires:
.PHONY : benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.requires

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.provides: benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.requires
	$(MAKE) -f benchmarks/CMakeFiles/expand2b.dir/build.make benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.provides.build
.PHONY : benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.provides

benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.provides.build: benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o

# Object files for target expand2b
expand2b_OBJECTS = \
"CMakeFiles/expand2b.dir/expand2b.cpp.o"

# External object files for target expand2b
expand2b_EXTERNAL_OBJECTS =

benchmarks/expand2b: benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o
benchmarks/expand2b: benchmarks/CMakeFiles/expand2b.dir/build.make
benchmarks/expand2b: symengine/libsymengine.a
benchmarks/expand2b: symengine/utilities/teuchos/libteuchos.a
benchmarks/expand2b: /usr/lib/x86_64-linux-gnu/libgmpxx.so
benchmarks/expand2b: /usr/lib/x86_64-linux-gnu/libgmp.so
benchmarks/expand2b: benchmarks/CMakeFiles/expand2b.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable expand2b"
	cd /home/akash/symengine/benchmarks && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/expand2b.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
benchmarks/CMakeFiles/expand2b.dir/build: benchmarks/expand2b
.PHONY : benchmarks/CMakeFiles/expand2b.dir/build

benchmarks/CMakeFiles/expand2b.dir/requires: benchmarks/CMakeFiles/expand2b.dir/expand2b.cpp.o.requires
.PHONY : benchmarks/CMakeFiles/expand2b.dir/requires

benchmarks/CMakeFiles/expand2b.dir/clean:
	cd /home/akash/symengine/benchmarks && $(CMAKE_COMMAND) -P CMakeFiles/expand2b.dir/cmake_clean.cmake
.PHONY : benchmarks/CMakeFiles/expand2b.dir/clean

benchmarks/CMakeFiles/expand2b.dir/depend:
	cd /home/akash/symengine && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/akash/symengine /home/akash/symengine/benchmarks /home/akash/symengine /home/akash/symengine/benchmarks /home/akash/symengine/benchmarks/CMakeFiles/expand2b.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : benchmarks/CMakeFiles/expand2b.dir/depend

