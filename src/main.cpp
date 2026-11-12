#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include <FlexLexer.h>
#include "ast/DotWriter.h"
#include "ast/ParseResult.h"
#include "ast/AstValidator.h"
#include "lua_parser.hpp"
#include "semantic/SemanticAnalyzer.h"
#include "semantic/SemanticDotWriter.h"
#include "semantic/SemanticTransformer.h"

namespace {

yyFlexLexer* activeLexer = nullptr;

class NullBuffer final : public std::streambuf
{
protected:
    int overflow(int character) override
    {
        return character;
    }
};

}

int yylex()
{
    return activeLexer->yylex();
}

int main(int argc, char* argv[])
{
    const bool writesDot = argc == 4 && (std::string(argv[2]) == "--ast-dot" || std::string(argv[2]) == "--semantic-dot");
    if (argc != 2 && !writesDot) {
        std::cerr << "Usage: " << argv[0] << " <file> [--ast-dot|--semantic-dot <output.dot>]" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    yyFlexLexer lexer(&input, &std::cout);
    activeLexer = &lexer;
    lua::ast::clearParsedBlock();

    NullBuffer nullBuffer;
    std::streambuf* originalOutput = std::cout.rdbuf(&nullBuffer);
    int parseStatus = yyparse();
    std::cout.rdbuf(originalOutput);

    if (parseStatus == 0) {
        auto block = lua::ast::takeParsedBlock();
        lua::semantic::SemanticTransformer transformer;
        transformer.normalize(*block);
        lua::ast::AstValidator validator;
        const lua::ast::AstValidationResult validationResult = validator.validate(*block);
        if (!validationResult.isValid()) {
            for (const lua::ast::AstDiagnostic& diagnostic : validationResult.diagnostics()) {
                std::cerr << "AST error: " << diagnostic.message << std::endl;
            }
            return 1;
        }
        lua::semantic::SemanticAnalyzer analyzer;
        const lua::semantic::SemanticResult semanticResult = analyzer.analyze(*block);

        if (!semanticResult.diagnostics().empty()) {
            for (const lua::semantic::Diagnostic& diagnostic : semanticResult.diagnostics()) {
                std::cerr << "Semantic error: " << diagnostic.message << std::endl;
            }
            return 1;
        }

        std::cout << "Syntax is correct\n";

        if (writesDot) {
            std::ofstream output(argv[3]);
            if (!output.is_open()) {
                std::cerr << "Unable to open output file: " << argv[3] << std::endl;
                return 1;
            }

            if (std::string(argv[2]) == "--semantic-dot") {
                lua::semantic::SemanticDotWriter::writeTree(output, *block, semanticResult);
            } else {
                lua::ast::DotWriter::writeTree(output, *block, "ast");
            }
        }
    }

    return parseStatus;
}
