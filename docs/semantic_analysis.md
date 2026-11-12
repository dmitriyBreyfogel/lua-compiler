# Semantic analysis

Semantic analysis starts after parsing and before any future bytecode generation.
It does not change the approved grammar.

The compiler builds one AST node class from `include/ast/Node.h`. Its `NodeType`
enum distinguishes blocks, statements, declarations, expressions and literals.
Every node stores an optional string value and named children.

The parser creates nodes through `createNode`. Roles such as `"left"`,
`"condition"` and `"body"` describe the relationship between a node and its
children. Semantic code reads them with `Node::child`.

Before semantic analysis, `AstValidator` checks the root node, required children
and child categories. `SemanticTransformer` then supplies the omitted `step`
expression of a numeric `for` loop with literal `1`.

`SemanticAnalyzer` performs the language checks and produces a `SemanticResult`:

- creates symbols for locals, globals, parameters, loop variables and labels;
- resolves identifier bindings and records captured locals as upvalues;
- creates nested scopes through `Scope`;
- checks `break`, vararg use, labels and `goto` scope jumps;
- rejects writes to `<const>`, `<close>` and loop variables;
- supplies implicit `self` for method declarations.

Run the complete relevant suite after a build:

```sh
ctest --test-dir build --output-on-failure
sh test/parser/run_parser_tests.sh build/lua_compiler
sh test/semantic/run_semantic_tests.sh build/lua_compiler
```

For a manual representation of bindings and scopes, use semantic DOT/SVG as
described in [AST visualization](ast_visualization.md).
