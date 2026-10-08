#!/usr/bin/env bash
# Regression guard: ownership-gated optimizations must use the backend-neutral
# is_uniquely_owned() predicate, not use_count() == 1. The latter is wrong for
# externally owned cooperative objects and advisory under concurrent changes.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Find all use_count() == 1 occurrences in .cpp/.h files under symengine/,
# excluding test files.
matches=$(grep -rn 'use_count()[[:space:]]*==[[:space:]]*1' \
    --include='*.cpp' --include='*.h' \
    "$SRC_ROOT" \
    | grep -v '/tests/' \
    || true)

if [ -n "$matches" ]; then
    echo "ERROR: Found use_count() == 1 ownership gate(s):"
    echo "$matches"
    echo ""
    echo "Use is_uniquely_owned() instead."
    exit 1
fi

echo "OK: No use_count() == 1 ownership gates found."
