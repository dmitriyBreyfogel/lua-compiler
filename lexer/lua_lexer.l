%option c++
%option noyywrap
%option yylineno
%option nodefault

%{
#include <cstdlib>
#include <iostream>
%}

NAME_START    [A-Za-z_]
NAME_SYMBOL   [A-Za-z0-9_]

DECIMAL_DIGIT          [0-9]
HEXADECIMAL_DIGIT      [0-9A-Fa-f]

DECIMAL_EXPONENT       [eE][+-]?{DECIMAL_DIGIT}+
DECIMAL_INTEGER        {DECIMAL_DIGIT}+
DECIMAL_FRACTION       (({DECIMAL_DIGIT}+\.{DECIMAL_DIGIT}*)|(\.{DECIMAL_DIGIT}+))
DECIMAL_FLOAT          ({DECIMAL_FRACTION}{DECIMAL_EXPONENT}?|{DECIMAL_DIGIT}+{DECIMAL_EXPONENT})

HEXADECIMAL_PREFIX     0[xX]
HEXADECIMAL_INTEGER    {HEXADECIMAL_PREFIX}{HEXADECIMAL_DIGIT}+
HEXADECIMAL_EXPONENT   [pP][+-]?{DECIMAL_DIGIT}+
HEXADECIMAL_FRACTION   (({HEXADECIMAL_DIGIT}+\.{HEXADECIMAL_DIGIT}*)|(\.{HEXADECIMAL_DIGIT}+))
HEXADECIMAL_FLOAT      ({HEXADECIMAL_PREFIX}{HEXADECIMAL_FRACTION}{HEXADECIMAL_EXPONENT}?|{HEXADECIMAL_INTEGER}{HEXADECIMAL_EXPONENT})

%%

"and"       { std::cout << "Found keyword: " << yytext << "\n"; }
"break"     { std::cout << "Found keyword: " << yytext << "\n"; }
"do"        { std::cout << "Found keyword: " << yytext << "\n"; }
"else"      { std::cout << "Found keyword: " << yytext << "\n"; }
"elseif"    { std::cout << "Found keyword: " << yytext << "\n"; }
"end"       { std::cout << "Found keyword: " << yytext << "\n"; }
"false"     { std::cout << "Found keyword: " << yytext << "\n"; }
"for"       { std::cout << "Found keyword: " << yytext << "\n"; }
"function"  { std::cout << "Found keyword: " << yytext << "\n"; }
"global"    { std::cout << "Found keyword: " << yytext << "\n"; }
"goto"      { std::cout << "Found keyword: " << yytext << "\n"; }
"if"        { std::cout << "Found keyword: " << yytext << "\n"; }
"in"        { std::cout << "Found keyword: " << yytext << "\n"; }
"local"     { std::cout << "Found keyword: " << yytext << "\n"; }
"nil"       { std::cout << "Found keyword: " << yytext << "\n"; }
"not"       { std::cout << "Found keyword: " << yytext << "\n"; }
"or"        { std::cout << "Found keyword: " << yytext << "\n"; }
"repeat"    { std::cout << "Found keyword: " << yytext << "\n"; }
"return"    { std::cout << "Found keyword: " << yytext << "\n"; }
"then"      { std::cout << "Found keyword: " << yytext << "\n"; }
"true"      { std::cout << "Found keyword: " << yytext << "\n"; }
"until"     { std::cout << "Found keyword: " << yytext << "\n"; }
"while"     { std::cout << "Found keyword: " << yytext << "\n"; }

{HEXADECIMAL_FLOAT} {
    std::cout << "Found hexadecimal float: " << std::strtod(yytext, nullptr) << "\n";
}

{HEXADECIMAL_INTEGER} {
    std::cout << "Found hexadecimal integer: " << std::strtoll(yytext, nullptr, 16) << "\n";
}

{DECIMAL_FLOAT} {
    std::cout << "Found float: " << std::strtod(yytext, nullptr) << "\n";
}

{DECIMAL_INTEGER} {
    std::cout << "Found integer: " << std::strtoll(yytext, nullptr, 10) << "\n";
}

{NAME_START}{NAME_SYMBOL}* {
    std::cout << "Found identifier: " << yytext << "\n";
}

.|\n        { std::cout << "Unknown token: " << yytext << "\n"; }

%%
