cmake_minimum_required(VERSION 3.20)

project(lua_compiler
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(FLEX_EXECUTABLE flex REQUIRED)

set(GENERATED_LEXER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_lexer.cpp")

add_custom_command(
    OUTPUT "${GENERATED_LEXER_FILE}"
    COMMAND "${FLEX_EXECUTABLE}" --c++ -o "${GENERATED_LEXER_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
    VERBATIM
)

add_executable(lua_compiler
    src/main.cpp
    "${GENERATED_LEXER_FILE}"
)

set_source_files_properties("${GENERATED_LEXER_FILE}" PROPERTIES LANGUAGE CXX)
