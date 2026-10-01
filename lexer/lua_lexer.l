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

/* Однострочный комментарий */
%x SINGLE_LINE_COMMENT

%{
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>

/* Возвращает числовое значение шестнадцатеричной цифры */
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

/* Преобразует шестнадцатеричное число в целое число с учетом переполнения */
static long long parseHexadecimalInteger(const char* text)
{
    unsigned long long value = 0;

    for (const char* character = text + 2; *character != '\0'; ++character) {
        value = value * 16 + static_cast<unsigned long long>(hexDigitValue(*character));
    }

    return static_cast<long long>(value);
}

/* Добавляет кодовую точку Unicode в строковый буфер */
static void appendUtf8(unsigned long long value, std::string& stringBuffer)
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
        appendUtf8(0xFFFD, stringBuffer);
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
    bool isHexadecimal = number.size() >= 2 && number[0] == '0' && (number[1] == 'x' || number[1] == 'X');
    bool isFloat = number.find('.') != std::string::npos || (isHexadecimal
                    ? number.find_first_of("pP") != std::string::npos
                    : number.find_first_of("eE") != std::string::npos);

    if (isHexadecimal && !isFloat) {
        std::cout << "Found hexadecimal integer: " << parseHexadecimalInteger(text) << "\n";
    } 
    else if (isHexadecimal) {
        std::cout << "Found hexadecimal float: " << value << "\n";
    } 
    else if (isFloat) {
        std::cout << "Found float: " << value << "\n";
    } 
    else {
        errno = 0;
        long long integer = std::strtoll(text, nullptr, 10);

        if (errno == ERANGE) {
            std::cout << "Found float: " << value << "\n";
            return;
        }

        std::cout << "Found integer: " << integer << "\n";
    }
}
%}

/* Макросы */
NAME_START    [A-Za-z_]
NAME_SYMBOL   [A-Za-z0-9_]
WHITESPACES   [ \t\r\n\f\v]+


/* Корректный шестнадцатеричный числовой литерал */
HEX_NUMERAL          0[xX]([0-9A-Fa-f]+(\.[0-9A-Fa-f]*)?|\.[0-9A-Fa-f]+)([pP][+-]?[0-9]+)?
/* Последовательность, похожая на шестнадцатеричный литерал, но с неверным форматом */
INVALID_HEX_NUMERAL  0[xX]([0-9A-Fa-f.]|[pP][+-]?)*([A-Za-z_][A-Za-z0-9_]*)?
DECIMAL_MANTISSA         ([0-9]+\.[0-9]*|[0-9]+|\.[0-9]+)
DECIMAL_EXPONENT         [eE][+-]?[0-9]+
DECIMAL_NUMERAL          {DECIMAL_MANTISSA}({DECIMAL_EXPONENT})?
INVALID_DECIMAL_NUMERAL  ({DECIMAL_MANTISSA}{DECIMAL_EXPONENT}[A-Za-z_][A-Za-z0-9_]*|{DECIMAL_MANTISSA}[A-DF-Za-df-z_][A-Za-z0-9_]*|{DECIMAL_MANTISSA}[eE][+-]?([A-Za-z_][A-Za-z0-9_]*)?|[0-9]+\.[0-9]+\.[0-9][0-9A-Za-z_.]*)

%%

    /* -------- Локальное состояние одного запуска yylex -------- */
    std::string stringBuffer;
    std::string longClose;
    bool hasStringError = false;

 /* ------------------------------ Ключевые слова ------------------------------ */
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

 /* ---------------------------- Числовые литералы ---------------------------- */
 /* Шестнадцатеричный числовой литерал */
{HEX_NUMERAL}         { printNumberToken(yytext); }

 /* Некорректный шестнадцатеричный числовой литерал */
{INVALID_HEX_NUMERAL} {
    std::cout << "Error: malformed number: " << yytext << "\n";
}

 /* Десятичное целое перед оператором конкатенации */
[0-9]+/\.\. {
    printNumberToken(yytext);
}

 /* Некорректный десятичный числовой литерал */
{INVALID_DECIMAL_NUMERAL} {
    std::cout << "Error: malformed number: " << yytext << "\n";
}

 /* Десятичный числовой литерал */
{DECIMAL_NUMERAL}     { printNumberToken(yytext); }

 /* -------------------------------- Комментарии -------------------------------- */
 /* ----- Многострочный комментарий: открытие, текст и закрытие ----- */
"--"\[=*\[(\r\n|\n\r|\r|\n)? {
    size_t equalsCount = 0;

    for (size_t index = 3; yytext[index] == '='; ++index) {
        ++equalsCount;
    }

    longClose = "]";
    longClose.append(equalsCount, '=');
    longClose.push_back(']');
    stringBuffer.clear();
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

 /* ----- Однострочный комментарий: открытие, текст и закрытие ----- */
"--" {
    stringBuffer = "--";
    BEGIN(SINGLE_LINE_COMMENT);
}

<SINGLE_LINE_COMMENT>[^\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

<SINGLE_LINE_COMMENT>\r\n|\n\r|\r|\n {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

<SINGLE_LINE_COMMENT><<EOF>> {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* ------------------------------ Длинные строки ------------------------------ */
 /* ----- Некорректный и открывающий разделители ----- */
\[=+ {
    std::cout << "Error: invalid long string delimiter: " << yytext << "\n";
}

 /* Открывающий разделитель длинной строки */
\[=*\[(\r\n|\n\r|\r|\n)? {
    stringBuffer.clear();
    longClose = "]";
    std::size_t equalCount = 0;

    while (yytext[equalCount + 1] == '=') {
        ++equalCount;
    }

    longClose.append(equalCount, '=');
    longClose.push_back(']');
    BEGIN(LONG_STRING);
}

 /* ----- Содержимое строки и нормализация переводов строк ----- */
<LONG_STRING>[^\]\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* Возврат каретки и перевод строки внутри длинной строки */
<LONG_STRING>\r\n {
    stringBuffer.push_back('\n');
}

 /* Перевод строки и возврат каретки внутри длинной строки */
<LONG_STRING>\n\r {
    stringBuffer.push_back('\n');
}

 /* Возврат каретки внутри длинной строки */
<LONG_STRING>\r {
    stringBuffer.push_back('\n');
}

 /* Перевод строки внутри длинной строки */
<LONG_STRING>\n {
    stringBuffer.push_back('\n');
}

 /* ----- Закрытие строки или сообщение об ошибке ----- */
<LONG_STRING>\]=*\] {
    if (longClose == std::string(yytext, yyleng)) {
        std::cout << "Found long string: " << stringBuffer << "\n";
        BEGIN(INITIAL);
    } 
    else {
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

 /* ----------------------------- Короткие строки ----------------------------- */
 /* ----- Открытие строки и обычный текст ----- */
\"          { stringBuffer.clear(); hasStringError = false; BEGIN(DOUBLE_QUOTED_STRING); }

\'          { stringBuffer.clear(); hasStringError = false; BEGIN(SINGLE_QUOTED_STRING); }

<DOUBLE_QUOTED_STRING>[^"\\\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

<SINGLE_QUOTED_STRING>[^'\\\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* ----- Простые escape-последовательности ----- */
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

 /* ----- Шестнадцатеричные и десятичные escape-последовательности ----- */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x[0-9A-Fa-f]{2} {
    int high = hexDigitValue(yytext[2]);
    int low = hexDigitValue(yytext[3]);
    stringBuffer.push_back(static_cast<char>((high << 4) | low));
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x([0-9A-Fa-f]?[^0-9A-Fa-f"'\r\n][^"'\r\n]*)? {
    std::cout << "Error: invalid hexadecimal escape: " << yytext << "\n";
    hasStringError = true;
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x[0-9A-Fa-f]? {
    std::cout << "Error: invalid hexadecimal escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Десятичная escape-последовательность */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\[0-9]{1,3} {
    long value = std::strtol(yytext + 1, nullptr, 10);

    if (value > 255) {
        std::cout << "Error: decimal escape too large: " << yytext << "\n";
        hasStringError = true;
    } else {
        stringBuffer.push_back(static_cast<char>(value));
    }
}

 /* ----- Unicode escape-последовательности ----- */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]+\} {
    errno = 0;
    unsigned long long value = std::strtoull(yytext + 3, nullptr, 16);

    if (errno == ERANGE || value >= 0x80000000ULL) {
        std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
        hasStringError = true;
    } else {
        appendUtf8(value, stringBuffer);
    }
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]*[^0-9A-Fa-f}"'\r\n][^"'\r\n]* {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]*\} {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]* {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

 /* ----- Пропуск пробелов и экранированные переводы строк ----- */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\z[ \t\r\n\f\v]* {
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\r\n {
    stringBuffer.push_back('\n');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\n\r {
    stringBuffer.push_back('\n');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\r {
    stringBuffer.push_back('\n');
}

<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\n {
    stringBuffer.push_back('\n');
}

 /* ----- Ошибочные escape-последовательности и завершение строки ----- */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\. {
    std::cout << "Error: invalid string escape: " << yytext << "\n";
    hasStringError = true;
    stringBuffer.push_back(yytext[1]);
}

<DOUBLE_QUOTED_STRING>\r\n|\n\r|\r|\n {
    std::cout << "Error: unterminated double-quoted string\n";
    BEGIN(INITIAL);
}

<SINGLE_QUOTED_STRING>\r\n|\n\r|\r|\n {
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
    if (!hasStringError) {
        std::cout << "Found string: " << stringBuffer << "\n";
    }
    BEGIN(INITIAL);
}

<SINGLE_QUOTED_STRING>\' {
    if (!hasStringError) {
        std::cout << "Found string: " << stringBuffer << "\n";
    }
    BEGIN(INITIAL);
}

 /* ------------ Составные операторы и служебные последовательности ------------ */
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

 /* ------------------------- Односимвольные операторы ------------------------- */
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

 /* -------------------------------- Разделители -------------------------------- */
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

 /* ------------------------------ Идентификаторы ------------------------------ */
{NAME_START}{NAME_SYMBOL}* {
    std::cout << "Found identifier: " << yytext << "\n";
}

 /* ---------------------------- Пробельные символы ---------------------------- */
{WHITESPACES}    ;

 /* ---------------------------- Неизвестные символы ---------------------------- */
.|\n        { std::cout << "Error: unknown token: " << yytext << "\n"; }

%%
