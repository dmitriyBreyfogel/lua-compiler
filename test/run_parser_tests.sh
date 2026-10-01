#!/bin/sh

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-lua_compiler>" >&2
    exit 1
fi

compiler="$1"

expect_valid() {
    source="$1"

    if ! printf '%s\n' "$source" | "$compiler" /dev/stdin > /dev/null; then
        echo "FAIL (valid): $source" >&2
        exit 1
    fi
}

expect_invalid() {
    source="$1"

    if printf '%s\n' "$source" | "$compiler" /dev/stdin > /dev/null 2>&1; then
        echo "FAIL (invalid): $source" >&2
        exit 1
    fi
}

expect_valid 'local n = 0 while n < 3 do n = n + 1 end'
expect_valid 'while true do break; end'
expect_valid 'repeat local n = 1 until n > 0'
expect_valid 'for i = 1, 3 do print(i) end'
expect_valid 'for i = 3, 1, -1 do print(i) end'
expect_valid 'for key, value in pairs(items) do print(key, value) end'
expect_valid 'for i = 1, 3 do if i == 2 then break end print(i) end'

expect_invalid 'while true break end'
expect_invalid 'repeat local n = 1 end'
expect_invalid 'for i = 1 do end'
expect_invalid 'for key items do end'

echo 'Parser loop tests passed'
