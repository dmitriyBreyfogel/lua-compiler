#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"
actual_output="$(mktemp)"

cleanup() {
    rm -f "$actual_output"
}

trap cleanup EXIT HUP INT TERM

run_test() {
    test_name="$1"
    input_file="${test_name}.lua"
    expected_output="${test_name}.out"

    "$compiler" "$input_file" > "$actual_output"

    if cmp -s "$expected_output" "$actual_output"; then
        echo "PASS: $input_file"
        return
    fi

    echo "FAIL: $input_file" >&2
    diff -u "$expected_output" "$actual_output" || true
    exit 1
}

run_test test/simple_types/numbers
run_test test/simple_types/strings
run_test test/comments/comments
run_test test/keywords/keywords
run_test test/operators/operators
run_test test/identifiers/identifiers
run_test test/errors/invalid_escapes
run_test test/errors/unterminated_strings
run_test test/errors/unterminated_long_string
run_test test/errors/unterminated_multiline_comment
run_test test/errors/unknown_tokens
