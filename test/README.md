# Tests

```sh
docker compose run --rm dev cmake -S . -B build
docker compose run --rm dev cmake --build build
docker compose run --rm dev sh test/lexer/run_lexer_tests.sh ./build/lua_lexer
docker compose run --rm dev sh test/parser/run_parser_tests.sh ./build/lua_compiler
```

Parser fixtures in `test/parser/valid` must be accepted. Files in
`test/parser/invalid` must produce a syntax error.
