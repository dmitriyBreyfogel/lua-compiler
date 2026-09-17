#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"
actual_output="$(mktemp)"
generated_output="$(mktemp)"

cleanup() {
    rm -f "$actual_output" "$generated_output"
}

trap cleanup EXIT HUP INT TERM

compare_output() {
    input_file="$1"
    expected_output="$2"

    if cmp -s "$expected_output" "$actual_output"; then
        echo "PASS: $input_file"
        return
    fi

    echo "FAIL: $input_file" >&2
    diff -u "$expected_output" "$actual_output" || true
    exit 1
}

run_test() {
    test_name="$1"
    input_file="${test_name}.lua"
    expected_output="${test_name}.out"

    "$compiler" "$input_file" > "$actual_output"
    compare_output "$input_file" "$expected_output"
}

run_generated_test() {
    input_file="$1"

    "$compiler" "$input_file" > "$actual_output"
    compare_output "$input_file" "$generated_output"
}

run_control_escape_test() {
    printf '%b' 'Found string: \a\b\f\r\t\v\nFound string: \0000\0377\0001\0014\0173\nFound string: \00012\n' > "$generated_output"
    run_generated_test test/simple_types/control_escapes.lua
}

run_test test/simple_types/numbers
run_test test/simple_types/strings
run_test test/simple_types/single_quoted_strings
run_control_escape_test
run_test test/simple_types/numbers_in_code
run_test test/simple_types/strings_in_code
run_test test/comments/comments
run_test test/comments/comments_in_code
run_test test/keywords/keywords
run_test test/operators/operators
run_test test/operators/operators_in_code
run_test test/identifiers/identifiers
run_test test/errors/invalid_escapes
run_test test/errors/unterminated_strings
run_test test/errors/unterminated_long_string
run_test test/errors/unterminated_multiline_comment
run_test test/errors/unknown_tokens
run_test test/complex/mixed_tokens
run_test test/complex/control_flow
