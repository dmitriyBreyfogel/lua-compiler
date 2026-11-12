#include <fstream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include <FlexLexer.h>
#include "ast/DotWriter.h"
#include "ast/ParseResult.h"
#include "lua_parser.hpp"

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
    if (argc != 2 && (argc != 4 || std::string(argv[2]) != "--ast-dot")) {
        std::cerr << "Usage: " << argv[0] << " <file> [--ast-dot <output.dot>]" << std::endl;
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
        std::cout << "Syntax is correct\n";

        if (argc == 4) {
            auto block = lua::ast::takeParsedBlock();
            std::ofstream output(argv[3]);
            if (!output.is_open()) {
                std::cerr << "Unable to open output file: " << argv[3] << std::endl;
                return 1;
            }

            lua::ast::DotWriter::writeTree(output, *block, "ast");
        }
    }

    return parseStatus;
}
