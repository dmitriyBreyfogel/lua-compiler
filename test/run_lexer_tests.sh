#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"
actual_output="$(mktemp)"
generated_output="$(mktemp)"
generated_input="$(mktemp)"

cleanup() {
    rm -f "$actual_output" "$generated_output" "$generated_input"
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

run_unicode_boundary_test() {
    printf '%b' 'Found string: \0000\0177\0302\0200\0337\0277\0340\0240\0200\0357\0277\0277\0360\0220\0200\0200\0367\0277\0277\0277\0370\0210\0200\0200\0200\0373\0277\0277\0277\0277\0374\0204\0200\0200\0200\0200\0375\0277\0277\0277\0277\0277\n' > "$generated_output"
    run_generated_test test/simple_types/unicode_boundaries.lua
}

run_end_of_file_test() {
    printf '%s' '"unfinished double string' > "$generated_input"
    printf '%s\n' 'Error: unterminated double-quoted string before end of file' > "$generated_output"
    run_generated_test "$generated_input"

    printf '%s' "'unfinished single string" > "$generated_input"
    printf '%s\n' 'Error: unterminated single-quoted string before end of file' > "$generated_output"
    run_generated_test "$generated_input"

    printf '%s' '[=[unfinished long string' > "$generated_input"
    printf '%s\n' 'Error: unterminated long string before end of file' > "$generated_output"
    run_generated_test "$generated_input"

    printf '%s' '--[==[unfinished multiline comment' > "$generated_input"
    printf '%s\n' 'Error: unterminated multiline comment before end of file' > "$generated_output"
    run_generated_test "$generated_input"
}

run_line_ending_test() {
    printf '%b' 'first\rsecond\r\nthird\n\rfourth\ffifth\vsixth\tseventh' > "$generated_input"
    printf '%s\n' 'Found identifier: first' 'Found identifier: second' 'Found identifier: third' 'Found identifier: fourth' 'Found identifier: fifth' 'Found identifier: sixth' 'Found identifier: seventh' > "$generated_output"
    run_generated_test "$generated_input"

    printf '%b' '[[\r\nfirst\rsecond\n\rthird\nfourth]]' > "$generated_input"
    printf '%b' 'Found long string: first\nsecond\nthird\nfourth\n' > "$generated_output"
    run_generated_test "$generated_input"

    printf '%b' '"first\\\rsecond"\n"second\\\r\nthird"\n"third\\\n\rfourth"\n"before\\z \t\f\v\r\nafter"' > "$generated_input"
    printf '%b' 'Found string: first\nsecond\nFound string: second\nthird\nFound string: third\nfourth\nFound string: beforeafter\n' > "$generated_output"
    run_generated_test "$generated_input"
}

run_test test/simple_types/numbers
run_test test/simple_types/number_boundaries
run_test test/simple_types/strings
run_test test/simple_types/single_quoted_strings
run_test test/simple_types/long_delimiters
run_control_escape_test
run_unicode_boundary_test
run_end_of_file_test
run_line_ending_test
run_test test/simple_types/numbers_in_code
run_test test/simple_types/strings_in_code
run_test test/comments/comments
run_test test/comments/long_delimiters
run_test test/comments/comment_opening_boundaries
run_test test/comments/comments_in_code
run_test test/keywords/keywords
run_test test/operators/operators
run_test test/operators/operators_in_code
run_test test/identifiers/identifiers
run_test test/identifiers/keyword_boundaries
run_test test/errors/invalid_escapes
run_test test/errors/malformed_string_escapes
run_test test/errors/invalid_long_delimiters
run_test test/errors/unterminated_strings
run_test test/errors/unterminated_long_string
run_test test/errors/unterminated_multiline_comment
run_test test/errors/unknown_tokens
run_test test/complex/mixed_tokens
run_test test/complex/control_flow
