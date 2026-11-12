#!/bin/sh

set -eu

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <lua_compiler> <input.lua> <output-name>" >&2
    exit 1
fi

compiler="$1"
input="$2"
output_name="$3"
dot_file="$output_name.dot"
svg_file="$output_name.svg"

mkdir -p "$(dirname "$output_name")"

if ! command -v dot >/dev/null 2>&1; then
    echo "Graphviz is required. Install it with: brew install graphviz" >&2
    exit 1
fi

"$compiler" "$input" --ast-dot "$dot_file"
dot -Tsvg "$dot_file" -o "$svg_file"

echo "Created: $dot_file"
echo "Created: $svg_file"
