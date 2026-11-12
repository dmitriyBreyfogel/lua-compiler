# Semantic analysis

Semantic analysis starts after parsing and before any future bytecode generation.
It does not change the approved grammar.

The compiler builds concrete AST nodes in `include/ast`:

- `Node.h` owns common identity and children.
- `expression/` contains expressions: literals, identifiers, calls, tables,
  unary and binary operations, functions, fields and indexes.
- `statement/` contains blocks, declarations, assignments, branches, loops,
  calls, labels, gotos and returns.
- `SupportNodes.h` contains parser support nodes for lists, declared names and
  attributes.

The parser creates these classes through `makeNode`. Child-role strings are an
implementation detail of construction; semantic code uses typed accessors such
as `LoopStatement::body()` and `BinaryExpression::left()`.

Declarations and control flow are represented by separate concrete classes:
`LocalDeclarationStatement`, `GlobalDeclarationStatement`,
`GlobalAllDeclarationStatement`, `IfStatement`, `ElseIfStatement`,
`NumericForStatement`, `GenericForStatement`, `WhileStatement` and
`RepeatStatement`. Literals also have separate classes for integer, float,
string, boolean and nil values.

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
