cmake_minimum_required(VERSION 3.20)

project(lua_compiler
    VERSION 0.1.0
    LANGUAGES CXX
)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(FLEX_EXECUTABLE flex REQUIRED)
find_program(BISON_EXECUTABLE bison REQUIRED)

set(GENERATED_LEXER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_lexer.cpp")
set(GENERATED_PARSER_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_parser.cpp")
set(GENERATED_PARSER_HEADER "${CMAKE_CURRENT_BINARY_DIR}/lua_parser.hpp")
set(PARSER_REPORT_FILE "${CMAKE_CURRENT_BINARY_DIR}/lua_parser.output")

add_custom_command(
    OUTPUT
        "${GENERATED_PARSER_FILE}"
        "${GENERATED_PARSER_HEADER}"
        "${PARSER_REPORT_FILE}"
    COMMAND "${BISON_EXECUTABLE}"
        "--defines=${GENERATED_PARSER_HEADER}"
        "--output=${GENERATED_PARSER_FILE}"
        "--report=state"
        "--report-file=${PARSER_REPORT_FILE}"
        "-Werror=conflicts-sr,error=conflicts-rr"
        "${CMAKE_CURRENT_SOURCE_DIR}/parser/lua_parser.y"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/parser/lua_parser.y"
    VERBATIM
)

add_custom_target(generate_parser ALL
    DEPENDS
        "${GENERATED_PARSER_FILE}"
        "${GENERATED_PARSER_HEADER}"
        "${PARSER_REPORT_FILE}"
)

add_custom_command(
    OUTPUT "${GENERATED_LEXER_FILE}"
    COMMAND "${FLEX_EXECUTABLE}" --c++ -o "${GENERATED_LEXER_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lua_lexer.l"
        "${GENERATED_PARSER_HEADER}"
    VERBATIM
)

add_library(lua_ast
    src/ast/Node.cpp
    src/ast/Ast.cpp
    src/ast/AstValidator.cpp
    src/ast/SupportNodes.cpp
    src/ast/expression/ExpressionNode.cpp
    src/ast/expression/LiteralExpressions.cpp
    src/ast/expression/Expressions.cpp
    src/ast/statement/StatementNode.cpp
    src/ast/statement/AssignmentStatement.cpp
    src/ast/statement/BlockStatement.cpp
    src/ast/statement/BreakStatement.cpp
    src/ast/statement/ConditionalStatement.cpp
    src/ast/statement/DeclarationStatement.cpp
    src/ast/statement/ElseIfStatement.cpp
    src/ast/statement/EmptyStatement.cpp
    src/ast/statement/FunctionCallStatement.cpp
    src/ast/statement/FunctionDeclarationStatement.cpp
    src/ast/statement/GenericForStatement.cpp
    src/ast/statement/GlobalAllDeclarationStatement.cpp
    src/ast/statement/GlobalDeclarationStatement.cpp
    src/ast/statement/GotoStatement.cpp
    src/ast/statement/IfStatement.cpp
    src/ast/statement/LabelStatement.cpp
    src/ast/statement/LocalDeclarationStatement.cpp
    src/ast/statement/LoopStatement.cpp
    src/ast/statement/NumericForStatement.cpp
    src/ast/statement/RepeatStatement.cpp
    src/ast/statement/ReturnStatement.cpp
    src/ast/statement/WhileStatement.cpp
    src/ast/DotWriter.cpp
    src/ast/ParseResult.cpp
    src/semantic/SemanticAnalyzer.cpp
    src/semantic/Scope.cpp
    src/semantic/SemanticDotWriter.cpp
    src/semantic/SemanticTransformer.cpp
)
target_include_directories(lua_ast PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

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

target_link_libraries(lua_compiler PRIVATE lua_ast)
target_link_libraries(lua_lexer PRIVATE lua_ast)

set_source_files_properties(
    "${GENERATED_LEXER_FILE}"
    "${GENERATED_PARSER_FILE}"
    PROPERTIES LANGUAGE CXX
)

target_include_directories(lua_compiler PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(lua_lexer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

if(BUILD_TESTING)
    add_executable(ast_dot_writer_test
        test/ast/dot_writer_test.cpp
    )
    target_link_libraries(ast_dot_writer_test PRIVATE lua_ast)
    add_test(NAME ast_dot_writer COMMAND ast_dot_writer_test)

    add_executable(ast_node_factory_test
        test/ast/node_factory_test.cpp
    )
    target_link_libraries(ast_node_factory_test PRIVATE lua_ast)
    add_test(NAME ast_node_factory COMMAND ast_node_factory_test)

    add_executable(ast_validator_test test/ast/ast_validator_test.cpp)
    target_link_libraries(ast_validator_test PRIVATE lua_ast)
    add_test(NAME ast_validator COMMAND ast_validator_test)

    add_executable(semantic_analyzer_test test/semantic/semantic_analyzer_test.cpp)
    target_link_libraries(semantic_analyzer_test PRIVATE lua_ast)
    add_test(NAME semantic_analyzer COMMAND semantic_analyzer_test)

    add_executable(semantic_scope_test test/semantic/scope_test.cpp)
    target_link_libraries(semantic_scope_test PRIVATE lua_ast)
    add_test(NAME semantic_scope COMMAND semantic_scope_test)

    add_test(
        NAME ast_dot
        COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/test/ast/run_ast_dot_tests.sh" $<TARGET_FILE:lua_compiler>
    )
    set_tests_properties(ast_dot PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    add_test(
        NAME semantic
        COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/test/semantic/run_semantic_tests.sh" $<TARGET_FILE:lua_compiler>
    )
    set_tests_properties(semantic PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
