# Tests

```sh
docker compose run --rm dev cmake -S . -B build
docker compose run --rm dev cmake --build build
docker compose run --rm dev sh test/run_lexer_tests.sh ./build/lua_compiler
```
