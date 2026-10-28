#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"
failures=0

expect_success() {
    if "$compiler" "$1" >/dev/null 2>&1; then
        echo "PASS: $1"
    else
        echo "FAIL: $1" >&2
        failures=$((failures + 1))
    fi
}

expect_failure() {
    if "$compiler" "$1" >/dev/null 2>&1; then
        echo "FAIL: $1" >&2
        failures=$((failures + 1))
    else
        echo "PASS: $1"
    fi
}

expect_success test/parser/simple_assignments.lua
expect_success test/parser/expression_precedence.lua
expect_success test/parser/return_last.lua
expect_success test/parser/variables.lua
expect_success test/parser/function_calls.lua
expect_success test/parser/tables.lua
expect_success test/parser/trailing_separators.lua
expect_success test/parser/functions.lua
expect_success test/parser/if_statements.lua
expect_failure test/parser/missing_expression.lua
expect_failure test/parser/incomplete_expression.lua
expect_failure test/parser/statement_after_return.lua
expect_failure test/parser/incomplete_field.lua
expect_failure test/parser/incomplete_call.lua
expect_failure test/parser/incomplete_table.lua
expect_failure test/parser/incomplete_function.lua
expect_failure test/parser/missing_then.lua
expect_failure test/parser/repeated_separators.lua

if [ "$failures" -ne 0 ]; then
    echo "FAILED: $failures test(s)" >&2
    exit 1
fi
