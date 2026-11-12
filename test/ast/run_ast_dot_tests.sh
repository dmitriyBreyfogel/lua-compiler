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

node_id() {
    EXPECTED="$2" awk '
        BEGIN { expected = ENVIRON["EXPECTED"] }
        /^[[:space:]]*node_[0-9]+ \[label="/ {
            line = $0
            sub(/^[[:space:]]*node_/, "", line)
            sub(/ .*/, "", line)
            label = $0
            sub(/^.*label="/, "", label)
            sub(/"\];$/, "", label)
            if (label == expected) {
                print line
                exit
            }
        }
    ' "$1"
}

has_edge() {
    grep -F "node_$2 -> node_$3 [label=\"$4\"];" "$1" >/dev/null
}

check_case() {
    source="$1"
    checks="$2"
    output="$temporary_directory/$(basename "$source").dot"

    if ! "$compiler" "$source" --ast-dot "$output" >/dev/null 2>&1; then
        echo "FAIL: $source (compiler failed)" >&2
        failures=$((failures + 1))
        return
    fi

    if [ ! -s "$output" ]; then
        echo "FAIL: $source (DOT file is empty)" >&2
        failures=$((failures + 1))
        return
    fi

    while IFS='|' read -r kind first second third; do
        case "$kind" in
            ''|'#'*) continue ;;
            node)
                if [ -z "$(node_id "$output" "$first")" ]; then
                    echo "FAIL: $source (missing node: $first)" >&2
                    failures=$((failures + 1))
                fi
                ;;
            edge)
                source_id=$(node_id "$output" "$first")
                target_id=$(node_id "$output" "$third")
                if [ -z "$source_id" ] || [ -z "$target_id" ] || ! has_edge "$output" "$source_id" "$target_id" "$second"; then
                    echo "FAIL: $source (missing edge: $first --$second--> $third)" >&2
                    failures=$((failures + 1))
                fi
                ;;
            *)
                echo "FAIL: $checks (unknown check: $kind)" >&2
                failures=$((failures + 1))
                ;;
        esac
    done < "$checks"

    if [ "$failures" -eq 0 ]; then
        echo "PASS: $source"
    fi
}

for checks in test/ast/expected/*.checks; do
    name=$(basename "$checks" .checks)
    check_case "test/ast/input/$name.lua" "$checks"
done

if [ "$failures" -ne 0 ]; then
    echo "FAILED: $failures AST DOT check(s)" >&2
    exit 1
fi

echo "PASSED: AST DOT tests"
