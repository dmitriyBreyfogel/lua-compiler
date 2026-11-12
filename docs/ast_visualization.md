# AST visualization

DOT and SVG files are only for manual debugging of the tree built by the parser.

Build the compiler first:

```sh
cmake -S . -B build
cmake --build build
```

Install Graphviz once if the `dot` command is unavailable:

```sh
brew install graphviz
```

Build both files with one command:

```sh
sh tools/visualize_ast.sh ./build/lua_compiler test/ast/input/expressions.lua artifacts/expressions
```

The command creates:

```text
artifacts/expressions.dot
artifacts/expressions.svg
```

Open the SVG in a browser to zoom and inspect the tree. Edge labels describe the role of a child node, such as `left`, `right`, `condition`, `body`, or `arguments`.

To see semantic information, add `--semantic` before the compiler path:

```sh
sh tools/visualize_ast.sh --semantic ./build/lua_compiler test/semantic/valid/local_function.lua artifacts/local_function_semantic
```

Semantic SVG files include dashed links from identifiers to symbol nodes. A captured local variable is marked as `upvalue`.
