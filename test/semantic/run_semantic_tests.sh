#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"
failures=0
temporary_directory=$(mktemp -d)
trap 'rm -rf "$temporary_directory"' EXIT

expect_success() {
    if "$compiler" "$1" >"$temporary_directory/output" 2>&1; then
        echo "PASS: $1"
    else
        echo "FAIL: $1" >&2
        cat "$temporary_directory/output" >&2
        failures=$((failures + 1))
    fi
}

expect_error() {
    if "$compiler" "$1" >"$temporary_directory/output" 2>&1; then
        echo "FAIL: $1" >&2
        failures=$((failures + 1))
    elif grep -F "$2" "$temporary_directory/output" >/dev/null; then
        echo "PASS: $1"
    else
        echo "FAIL: $1 (missing error: $2)" >&2
        cat "$temporary_directory/output" >&2
        failures=$((failures + 1))
    fi
}

expect_success test/semantic/valid/local_function.lua
expect_success test/semantic/valid/named_vararg.lua
expect_success test/semantic/valid/method_self.lua
expect_error test/semantic/invalid/break_outside_loop.lua "break outside loop"
expect_error test/semantic/invalid/vararg_outside_function.lua "vararg outside function"
expect_error test/semantic/invalid/unknown_label.lua "unknown label: missing"
expect_error test/semantic/invalid/duplicate_label.lua "duplicate label: again"
expect_error test/semantic/invalid/goto_into_local_scope.lua "goto jumps into scope of local variable: ready"
expect_error test/semantic/invalid/assign_const.lua "assignment to read-only variable: value"
expect_error test/semantic/invalid/unknown_attribute.lua "unknown local attribute: unknown"
expect_error test/semantic/invalid/duplicate_parameter.lua "duplicate declaration: value"

if [ "$failures" -ne 0 ]; then
    echo "FAILED: $failures semantic test(s)" >&2
    exit 1
fi

echo "PASSED: semantic tests"
