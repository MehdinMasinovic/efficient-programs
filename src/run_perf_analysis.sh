#!/bin/bash

# Check if required arguments are provided
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <implementation_name> <implementation_command> [args...]"
    echo "Example: $0 reference 'myjoin f1.csv f2.csv f3.csv f4.csv'"
    exit 1
fi

IMPLEMENTATION_NAME=$1
shift  # Remove the first argument, leaving the command and its args
COMMAND="$@"

# Create temporary file for perf output
PERF_OUTPUT=$(mktemp)
echo "PERF_OUTPUT=$PERF_OUTPUT"

# Run perf with all metrics we want to track
echo "Running performance analysis for implementation: $IMPLEMENTATION_NAME"
LC_NUMERIC=en_US perf stat \
    -o "$PERF_OUTPUT" \
    -e cycles,instructions,branches,branch-misses \
    -e L1-dcache-load-misses,L1-dcache-loads,LLC-load-misses \
    -e cycles:k,cycles:u,instructions:k,instructions:u \
    $COMMAND |cat >/dev/null

# Parse the perf output and generate CSV
tail --lines=+2 "$PERF_OUTPUT" | python3 parse_perf_output.py "$IMPLEMENTATION_NAME" > "${IMPLEMENTATION_NAME}_performance.csv"

# Clean up temporary file

rm "$PERF_OUTPUT"

echo "Performance analysis complete. Results saved to ${IMPLEMENTATION_NAME}_performance.csv"
