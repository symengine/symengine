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
include benchmarks/CMakeFiles/lwbench.dir/depend.make

# Include the progress variables for this target.
include benchmarks/CMakeFiles/lwbench.dir/progress.make

# Include the compile flags for this target's objects.
include benchmarks/CMakeFiles/lwbench.dir/flags.make

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o: benchmarks/CMakeFiles/lwbench.dir/flags.make
benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o: benchmarks/lwbench.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/akash/symengine/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/lwbench.dir/lwbench.cpp.o -c /home/akash/symengine/benchmarks/lwbench.cpp

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lwbench.dir/lwbench.cpp.i"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/akash/symengine/benchmarks/lwbench.cpp > CMakeFiles/lwbench.dir/lwbench.cpp.i

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lwbench.dir/lwbench.cpp.s"
	cd /home/akash/symengine/benchmarks && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/akash/symengine/benchmarks/lwbench.cpp -o CMakeFiles/lwbench.dir/lwbench.cpp.s

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.requires:
.PHONY : benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.requires

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.provides: benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.requires
	$(MAKE) -f benchmarks/CMakeFiles/lwbench.dir/build.make benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.provides.build
.PHONY : benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.provides

benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.provides.build: benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o

# Object files for target lwbench
lwbench_OBJECTS = \
"CMakeFiles/lwbench.dir/lwbench.cpp.o"

# External object files for target lwbench
lwbench_EXTERNAL_OBJECTS =

benchmarks/lwbench: benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o
benchmarks/lwbench: benchmarks/CMakeFiles/lwbench.dir/build.make
benchmarks/lwbench: symengine/libsymengine.a
benchmarks/lwbench: symengine/utilities/teuchos/libteuchos.a
benchmarks/lwbench: /usr/lib/x86_64-linux-gnu/libgmpxx.so
benchmarks/lwbench: /usr/lib/x86_64-linux-gnu/libgmp.so
benchmarks/lwbench: benchmarks/CMakeFiles/lwbench.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable lwbench"
	cd /home/akash/symengine/benchmarks && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lwbench.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
benchmarks/CMakeFiles/lwbench.dir/build: benchmarks/lwbench
.PHONY : benchmarks/CMakeFiles/lwbench.dir/build

benchmarks/CMakeFiles/lwbench.dir/requires: benchmarks/CMakeFiles/lwbench.dir/lwbench.cpp.o.requires
.PHONY : benchmarks/CMakeFiles/lwbench.dir/requires

benchmarks/CMakeFiles/lwbench.dir/clean:
	cd /home/akash/symengine/benchmarks && $(CMAKE_COMMAND) -P CMakeFiles/lwbench.dir/cmake_clean.cmake
.PHONY : benchmarks/CMakeFiles/lwbench.dir/clean

benchmarks/CMakeFiles/lwbench.dir/depend:
	cd /home/akash/symengine && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/akash/symengine /home/akash/symengine/benchmarks /home/akash/symengine /home/akash/symengine/benchmarks /home/akash/symengine/benchmarks/CMakeFiles/lwbench.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : benchmarks/CMakeFiles/lwbench.dir/depend

