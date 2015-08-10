#!/bin/bash

# Note: use this script from the root dir of the ruby wrappers gem as
# your current dir.

# Memcheck is a memory error detector which can detect problems like
# accessing memory you shouldn't, using undefined values, incorrect
# freeing of heap memory, overlapping source and destination pointers
# in `memcpy` and related functions, memory leaks, etc.

# --tool=memcheck: specifies that memcheck is the tool to be used
# --num-callers=15: specifies the amount of stack-trace to show. Can be
#                   set to larger values if the stack trace is not big
#                   enough
# partial-loads-ok and undef-value-errors are necessary to ignore
# spurious (and eminently ignorable) warnings from the ruby
# interpreter. The MRI does a lot of funky things in the name of speed,
# like using uninitialized variables and reading past the ends of
# malloced blocks that aren't on an 8-byte boundary. For advanced info:
# http://valgrind.org/docs/manual/manual-core.html#manual-core.suppress
valgrind \
    --tool=memcheck \
    --num-callers=15 \
    --partial-loads-ok=yes \
    --undef-value-errors=no \
    ruby -S rspec spec

# additional options like `--leak-check=full` can be used if needed.
# If --leak-check=full is specified, Memcheck will give details for
# each definitely lost or possibly lost block, including where it was
# allocated.
