%option c++
%option noyywrap
%option yylineno
%option nodefault

/* Строки в двойных кавычках */
%x DOUBLE_QUOTED_STRING

/* Строки в одинарных кавычках */
%x SINGLE_QUOTED_STRING

/* Длинные строки */
%x LONG_STRING

/* Многострочные комментарии */
%x MULTILINE_COMMENT

%{
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>

/* Буферы для составных лексем */
static std::string stringBuffer;
static std::string longClose;

/* Возвращает числовое значение hex-цифры */
static int hexDigitValue(char symbol)
{
    if (symbol >= '0' && symbol <= '9') {
        return symbol - '0';
    }

    if (symbol >= 'a' && symbol <= 'f') {
        return 10 + symbol - 'a';
    }

    if (symbol >= 'A' && symbol <= 'F') {
        return 10 + symbol - 'A';
    }

    return -1;
}

/* Добавляет Unicode code point в строковый буфер */
static void appendUtf8(unsigned long long value)
{
    if (value <= 0x7F) {
        stringBuffer.push_back(static_cast<char>(value));
    } else if (value <= 0x7FF) {
        stringBuffer.push_back(static_cast<char>(0xC0 | ((value >> 6) & 0x1F)));
        stringBuffer.push_back(static_cast<char>(0x80 | (value & 0x3F)));
    } else if (value <= 0xFFFF) {
        stringBuffer.push_back(static_cast<char>(0xE0 | ((value >> 12) & 0x0F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | (value & 0x3F)));
    } else if (value <= 0x1FFFFF) {
        stringBuffer.push_back(static_cast<char>(0xF0 | ((value >> 18) & 0x07)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | (value & 0x3F)));
    } else if (value <= 0x3FFFFFF) {
        stringBuffer.push_back(static_cast<char>(0xF8 | ((value >> 24) & 0x03)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 18) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | (value & 0x3F)));
    } else if (value <= 0x7FFFFFFF) {
        stringBuffer.push_back(static_cast<char>(0xFC | ((value >> 30) & 0x01)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 24) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 18) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 12) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | ((value >> 6) & 0x3F)));
        stringBuffer.push_back(static_cast<char>(0x80 | (value & 0x3F)));
    } else {
        appendUtf8(0xFFFD);
    }
}

/* Проверяет и выводит числовую лексему Lua */
static void printNumberToken(const char* text)
{
    char* end = nullptr;
    double value = std::strtod(text, &end);

    if (*end != '\0') {
        std::cout << "Error: malformed number: " << text << "\n";
        return;
    }

    std::string number(text);
    bool isHexadecimal = number.size() >= 2
        && number[0] == '0'
        && (number[1] == 'x' || number[1] == 'X');
    bool isFloat = number.find('.') != std::string::npos
        || (isHexadecimal
            ? number.find_first_of("pP") != std::string::npos
            : number.find_first_of("eE") != std::string::npos);

    if (isHexadecimal && !isFloat) {
        std::cout << "Found hexadecimal integer: "
                  << std::strtoll(text, nullptr, 16) << "\n";
    } else if (isHexadecimal) {
        std::cout << "Found hexadecimal float: " << value << "\n";
    } else if (isFloat) {
        std::cout << "Found float: " << value << "\n";
    } else {
        std::cout << "Found integer: "
                  << std::strtoll(text, nullptr, 10) << "\n";
    }
}
%}

/* Макросы */
NAME_START    [A-Za-z_]
NAME_SYMBOL   [A-Za-z0-9_]
WHITESPACES   [ \t\r\n\f\v]+

HEX_NUMERAL          0[xX]([0-9A-Fa-f.]|[pP][+-]?)*([A-Za-z_][A-Za-z0-9_]*)?
DECIMAL_NUMERAL      [0-9]([0-9A-Fa-f.]|[eE][+-]?)*([A-Za-z_][A-Za-z0-9_]*)?
DOT_DECIMAL_NUMERAL  \.[0-9]([0-9A-Fa-f.]|[eE][+-]?)*([A-Za-z_][A-Za-z0-9_]*)?

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

{HEX_NUMERAL}         { printNumberToken(yytext); }
{DECIMAL_NUMERAL}     { printNumberToken(yytext); }
{DOT_DECIMAL_NUMERAL} { printNumberToken(yytext); }

--\[=*\[ {
    stringBuffer.clear();
    longClose = "]";
    longClose.append(yyleng - 4, '=');
    longClose.push_back(']');
    BEGIN(MULTILINE_COMMENT);
}

<MULTILINE_COMMENT>[^\]]+ {
    stringBuffer.append(yytext, yyleng);
}

<MULTILINE_COMMENT>\]=*\] {
    if (longClose == std::string(yytext, yyleng)) {
        std::cout << "Found multiline comment: " << stringBuffer << "\n";
        BEGIN(INITIAL);
    } else {
        stringBuffer.append(yytext, yyleng);
    }
}

<MULTILINE_COMMENT>\] {
    stringBuffer.push_back(']');
}

<MULTILINE_COMMENT><<EOF>> {
    std::cout << "Error: unterminated multiline comment before end of file\n";
    BEGIN(INITIAL);
}

--[^\r\n]*  { std::cout << "Found single-line comment: " << yytext << "\n"; }

\[=*\[ {
    stringBuffer.clear();
    longClose = "]";
    longClose.append(yyleng - 2, '=');
    longClose.push_back(']');
    BEGIN(LONG_STRING);
}

<LONG_STRING>[^\]]+ {
    stringBuffer.append(yytext, yyleng);
}

<LONG_STRING>\]=*\] {
    if (longClose == std::string(yytext, yyleng)) {
        std::cout << "Found long string: " << stringBuffer << "\n";
        BEGIN(INITIAL);
    } else {
        stringBuffer.append(yytext, yyleng);
    }
}

<LONG_STRING>\] {
    stringBuffer.push_back(']');
}

<LONG_STRING><<EOF>> {
    std::cout << "Error: unterminated long string before end of file\n";
    BEGIN(INITIAL);
}

\"          { stringBuffer.clear(); BEGIN(DOUBLE_QUOTED_STRING); }
\'          { stringBuffer.clear(); BEGIN(SINGLE_QUOTED_STRING); }

<DOUBLE_QUOTED_STRING>[^"\\\n]+ {
    stringBuffer.append(yytext, yyleng);
}

<SINGLE_QUOTED_STRING>[^'\\\n]+ {
    stringBuffer.append(yytext, yyleng);
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\a {
    stringBuffer.push_back('\a');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\b {
    stringBuffer.push_back('\b');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\f {
    stringBuffer.push_back('\f');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\n {
    stringBuffer.push_back('\n');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\r {
    stringBuffer.push_back('\r');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\t {
    stringBuffer.push_back('\t');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\v {
    stringBuffer.push_back('\v');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\\ {
    stringBuffer.push_back('\\');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\" {
    stringBuffer.push_back('"');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\' {
    stringBuffer.push_back('\'');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x[0-9A-Fa-f]{2} {
    int high = hexDigitValue(yytext[2]);
    int low = hexDigitValue(yytext[3]);
    stringBuffer.push_back(static_cast<char>((high << 4) | low));
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\[0-9]{1,3} {
    long value = std::strtol(yytext + 1, nullptr, 10);

    if (value > 255) {
        value = 255;
    }

    stringBuffer.push_back(static_cast<char>(value));
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]+\} {
    errno = 0;
    unsigned long long value = std::strtoull(yytext + 3, nullptr, 16);

    if (errno == ERANGE || value >= 0x80000000ULL) {
        std::cout << "Invalid unicode escape: " << yytext << "\n";
    } else {
        appendUtf8(value);
    }
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\z[ \t\r\n\f\v]* {
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\. {
    std::cout << "Error: invalid string escape: " << yytext << "\n";
    stringBuffer.push_back(yytext[1]);
}

<DOUBLE_QUOTED_STRING>\n {
    std::cout << "Error: unterminated double-quoted string\n";
    BEGIN(INITIAL);
}

<SINGLE_QUOTED_STRING>\n {
    std::cout << "Error: unterminated single-quoted string\n";
    BEGIN(INITIAL);
}

<DOUBLE_QUOTED_STRING><<EOF>> {
    std::cout << "Error: unterminated double-quoted string before end of file\n";
    BEGIN(INITIAL);
}

<SINGLE_QUOTED_STRING><<EOF>> {
    std::cout << "Error: unterminated single-quoted string before end of file\n";
    BEGIN(INITIAL);
}

<DOUBLE_QUOTED_STRING>\" {
    std::cout << "Found string: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

<SINGLE_QUOTED_STRING>\' {
    std::cout << "Found string: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

"..."       { std::cout << "Found varargs: " << yytext << "\n"; }
".."        { std::cout << "Found operator: " << yytext << "\n"; }
"//"        { std::cout << "Found operator: " << yytext << "\n"; }
"<<"        { std::cout << "Found operator: " << yytext << "\n"; }
">>"        { std::cout << "Found operator: " << yytext << "\n"; }
"=="        { std::cout << "Found operator: " << yytext << "\n"; }
"~="        { std::cout << "Found operator: " << yytext << "\n"; }
"<="        { std::cout << "Found operator: " << yytext << "\n"; }
">="        { std::cout << "Found operator: " << yytext << "\n"; }
"::"        { std::cout << "Found label separator: " << yytext << "\n"; }

"+"         { std::cout << "Found operator: " << yytext << "\n"; }
"-"         { std::cout << "Found operator: " << yytext << "\n"; }
"*"         { std::cout << "Found operator: " << yytext << "\n"; }
"/"         { std::cout << "Found operator: " << yytext << "\n"; }
"%"         { std::cout << "Found operator: " << yytext << "\n"; }
"^"         { std::cout << "Found operator: " << yytext << "\n"; }
"#"         { std::cout << "Found operator: " << yytext << "\n"; }
"&"         { std::cout << "Found operator: " << yytext << "\n"; }
"~"         { std::cout << "Found operator: " << yytext << "\n"; }
"|"         { std::cout << "Found operator: " << yytext << "\n"; }
"="         { std::cout << "Found operator: " << yytext << "\n"; }
"<"         { std::cout << "Found operator: " << yytext << "\n"; }
">"         { std::cout << "Found operator: " << yytext << "\n"; }

"("         { std::cout << "Found left parenthesis: " << yytext << "\n"; }
")"         { std::cout << "Found right parenthesis: " << yytext << "\n"; }
"["         { std::cout << "Found left bracket: " << yytext << "\n"; }
"]"         { std::cout << "Found right bracket: " << yytext << "\n"; }
"{"         { std::cout << "Found left brace: " << yytext << "\n"; }
"}"         { std::cout << "Found right brace: " << yytext << "\n"; }
";"         { std::cout << "Found semicolon: " << yytext << "\n"; }
":"         { std::cout << "Found colon: " << yytext << "\n"; }
","         { std::cout << "Found comma: " << yytext << "\n"; }
"."         { std::cout << "Found dot: " << yytext << "\n"; }

{NAME_START}{NAME_SYMBOL}* {
    std::cout << "Found identifier: " << yytext << "\n";
}

{WHITESPACES}    ;

.|\n        { std::cout << "Error: unknown token: " << yytext << "\n"; }

%%
