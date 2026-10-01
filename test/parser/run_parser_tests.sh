#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
test_dir="$(cd "$(dirname "$0")" && pwd)"
output="$(mktemp)"
failures=0
cases=0

trap 'rm -f "$output"' EXIT HUP INT TERM

for source in "$test_dir"/valid/*.lua; do
    [ -f "$source" ] || continue
    cases=$((cases + 1))

    if "$compiler" "$source" > "$output" 2>&1 && grep -qx 'Syntax is correct' "$output"; then
        echo "PASS: valid/$(basename "$source")"
    else
        echo "FAIL: valid/$(basename "$source")" >&2
        cat "$output" >&2
        failures=$((failures + 1))
    fi
done

for source in "$test_dir"/invalid/*.lua; do
    [ -f "$source" ] || continue
    cases=$((cases + 1))

    if "$compiler" "$source" > "$output" 2>&1; then
        echo "FAIL: invalid/$(basename "$source")" >&2
        cat "$output" >&2
        failures=$((failures + 1))
    else
        echo "PASS: invalid/$(basename "$source")"
    fi
done

if [ "$cases" -eq 0 ]; then
    echo 'No parser tests found' >&2
    exit 1
fi

if [ "$failures" -ne 0 ]; then
    echo "FAILED: $failures of $cases test(s)" >&2
    exit 1
fi

echo "PASSED: $cases parser test(s)"
