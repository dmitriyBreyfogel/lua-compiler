cmake_minimum_required(VERSION 3.20)

project(lua_compiler
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(FLEX_EXECUTABLE flex REQUIRED)
find_program(BISON_EXECUTABLE bison REQUIRED)

set(GENERATED_LEXER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_lexer.cpp")
set(GENERATED_PARSER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_parser.cpp")
set(GENERATED_PARSER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/lua_parser.hpp")

add_custom_command(
    OUTPUT "${GENERATED_PARSER_FILE}" "${GENERATED_PARSER_HEADER}"
    COMMAND "${BISON_EXECUTABLE}"
        "--defines=${GENERATED_PARSER_HEADER}"
        "--output=${GENERATED_PARSER_FILE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/parser/lua_parser.y"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/parser/lua_parser.y"
    VERBATIM
)

add_custom_target(generate_parser ALL
    DEPENDS "${GENERATED_PARSER_FILE}" "${GENERATED_PARSER_HEADER}"
)

add_custom_command(
    OUTPUT "${GENERATED_LEXER_FILE}"
    COMMAND "${FLEX_EXECUTABLE}" --c++ -o "${GENERATED_LEXER_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
        "${GENERATED_PARSER_HEADER}"
    VERBATIM
)

add_executable(lua_compiler
    src/main.cpp
    "${GENERATED_LEXER_FILE}"
    "${GENERATED_PARSER_FILE}"
)

add_executable(lua_lexer
    src/lexer_main.cpp
    "${GENERATED_LEXER_FILE}"
    "${GENERATED_PARSER_FILE}"
)

set_source_files_properties(
    "${GENERATED_LEXER_FILE}"
    "${GENERATED_PARSER_FILE}"
    PROPERTIES LANGUAGE CXX
)

target_include_directories(lua_compiler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(lua_lexer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
