#include <fstream>
#include <iostream>

#include <FlexLexer.h>

namespace {

yyFlexLexer* activeLexer = nullptr;

}

int yylex()
{
    return activeLexer->yylex();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }

    yyFlexLexer lexer(&input, &std::cout);
    activeLexer = &lexer;

    while (lexer.yylex() != 0) {
    }

    return 0;
}
