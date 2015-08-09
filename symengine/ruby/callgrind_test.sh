#!/bin/bash

# Callgrind is a profiling tool that records the call history among functions
# in a program's run as a call-graph. By default, the collected data consists
# of the number of instructions executed, their relationship to source lines,
# the caller/callee relationship between functions, and the numbers of such
# calls.

#--tool=callgrind: ask valgrind to use callgrind profiler plugin
#--dump-instr=yes: specifies that event counting should be performed at per-instruction
#                   granularity. This allows for assembly code annotation. The results can
#                   only be displayed by KCachegrind.
#--simulate-cache=yes: runs with cache simulation
#--collect-jumps=yes: specifies whether information for (conditional) jumps should be
#                   collected. You have to use KCachegrind to get jump arrows in the
#                   annotated code.
valgrind \
    --tool=callgrind \
    --dump-instr=yes \
    --simulate-cache=yes \
    --collect-jumps=yes \
    ruby -S rspec spec

# After program termination, a profile data file named callgrind.out.<pid> is generated,
# where pid is the process ID of the program being profiled. The data file contains
# information about the calls made in the program among the functions executed

# To generate a function-by-function summary, use
# `callfrind_annotate callgrind.out.<pid>`
# or use Kcachegrind, the Call Graph Viewer.
# documentation: http://kcachegrind.sourceforge.net/html/Documentation.html
