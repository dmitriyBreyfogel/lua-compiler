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

/* Начало комментария */
%x COMMENT_AFTER_DASHES

/* Проверка разделителя многострочного комментария */
%x COMMENT_LONG_OPEN

/* Однострочный комментарий */
%x SINGLE_LINE_COMMENT

%{
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

/* Буферы для составных лексем */
static std::string stringBuffer;
static std::string longClose;
static bool hasStringError;

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

/* Преобразует шестнадцатеричное число в целое число Lua с оборачиванием */
static long long parseHexadecimalInteger(const char* text)
{
    unsigned long long value = 0;

    for (const char* character = text + 2; *character != '\0'; ++character) {
        value = value * 16 + static_cast<unsigned long long>(hexDigitValue(*character));
    }

    const unsigned long long maximumInteger =
        static_cast<unsigned long long>(std::numeric_limits<long long>::max());

    if (value <= maximumInteger) {
        return static_cast<long long>(value);
    }

    const unsigned long long maximumUnsignedInteger =
        std::numeric_limits<unsigned long long>::max();
    return -1 - static_cast<long long>(maximumUnsignedInteger - value);
}

/* Добавляет кодовую точку Unicode в строковый буфер */
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
                  << parseHexadecimalInteger(text) << "\n";
    } else if (isHexadecimal) {
        std::cout << "Found hexadecimal float: " << value << "\n";
    } else if (isFloat) {
        std::cout << "Found float: " << value << "\n";
    } else {
        errno = 0;
        long long integer = std::strtoll(text, nullptr, 10);

        if (errno == ERANGE) {
            std::cout << "Found float: " << value << "\n";
            return;
        }

        std::cout << "Found integer: "
                  << integer << "\n";
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

 /* Ключевые слова */
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
 /* Шестнадцатеричный числовой литерал */
{HEX_NUMERAL}         { printNumberToken(yytext); }

 /* Десятичный числовой литерал */
{DECIMAL_NUMERAL}     { printNumberToken(yytext); }

 /* Десятичный литерал, начинающийся с точки */
{DOT_DECIMAL_NUMERAL} { printNumberToken(yytext); }

 /* Начало комментария */
"--" {
    stringBuffer = "--";
    BEGIN(COMMENT_AFTER_DASHES);
}

 /* Открывающая скобка после -- */
<COMMENT_AFTER_DASHES>"[" {
    stringBuffer.push_back('[');
    BEGIN(COMMENT_LONG_OPEN);
}

 /* Первый символ однострочного комментария */
<COMMENT_AFTER_DASHES>. {
    stringBuffer.push_back(yytext[0]);
    BEGIN(SINGLE_LINE_COMMENT);
}

 /* Пустой комментарий до перевода строки */
<COMMENT_AFTER_DASHES>\r\n|\n\r|\r|\n {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Пустой комментарий до конца файла */
<COMMENT_AFTER_DASHES><<EOF>> {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Знак уровня многострочного комментария */
<COMMENT_LONG_OPEN>"=" {
    stringBuffer.push_back('=');
}

 /* Завершение открытия многострочного комментария */
<COMMENT_LONG_OPEN>"["(\r\n|\n\r|\r|\n)? {
    longClose = "]";
    longClose.append(stringBuffer.size() - 3, '=');
    longClose.push_back(']');
    stringBuffer.clear();
    BEGIN(MULTILINE_COMMENT);
}

 /* Некорректное открытие, переход к однострочному комментарию */
<COMMENT_LONG_OPEN>. {
    stringBuffer.push_back(yytext[0]);
    BEGIN(SINGLE_LINE_COMMENT);
}

 /* Незавершённое открытие до перевода строки */
<COMMENT_LONG_OPEN>\r\n|\n\r|\r|\n {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Незавершённое открытие до конца файла */
<COMMENT_LONG_OPEN><<EOF>> {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Текст однострочного комментария */
<SINGLE_LINE_COMMENT>[^\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* Завершение однострочного комментария переводом строки */
<SINGLE_LINE_COMMENT>\r\n|\n\r|\r|\n {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Завершение однострочного комментария концом файла */
<SINGLE_LINE_COMMENT><<EOF>> {
    std::cout << "Found single-line comment: " << stringBuffer << "\n";
    BEGIN(INITIAL);
}

 /* Текст многострочного комментария без закрывающей скобки */
<MULTILINE_COMMENT>[^\]]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* Возможный закрывающий разделитель многострочного комментария */
<MULTILINE_COMMENT>\]=*\] {
    if (longClose == std::string(yytext, yyleng)) {
        std::cout << "Found multiline comment: " << stringBuffer << "\n";
        BEGIN(INITIAL);
    } else {
        stringBuffer.append(yytext, yyleng);
    }
}

 /* Одиночная закрывающая скобка в многострочном комментарии */
<MULTILINE_COMMENT>\] {
    stringBuffer.push_back(']');
}

 /* Незакрытый многострочный комментарий */
<MULTILINE_COMMENT><<EOF>> {
    std::cout << "Error: unterminated multiline comment before end of file\n";
    BEGIN(INITIAL);
}

 /* Некорректный разделитель длинной строки */
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

 /* Текст длинной строки без закрывающей скобки */
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

 /* Возможный закрывающий разделитель длинной строки */
<LONG_STRING>\]=*\] {
    if (longClose == std::string(yytext, yyleng)) {
        std::cout << "Found long string: " << stringBuffer << "\n";
        BEGIN(INITIAL);
    } else {
        stringBuffer.append(yytext, yyleng);
    }
}

 /* Одиночная закрывающая скобка в длинной строке */
<LONG_STRING>\] {
    stringBuffer.push_back(']');
}

 /* Незакрытая длинная строка */
<LONG_STRING><<EOF>> {
    std::cout << "Error: unterminated long string before end of file\n";
    BEGIN(INITIAL);
}

 /* Открытие строки в двойных кавычках */
\"          { stringBuffer.clear(); hasStringError = false; BEGIN(DOUBLE_QUOTED_STRING); }

 /* Открытие строки в одинарных кавычках */
\'          { stringBuffer.clear(); hasStringError = false; BEGIN(SINGLE_QUOTED_STRING); }

 /* Обычный текст строки в двойных кавычках */
<DOUBLE_QUOTED_STRING>[^"\\\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* Обычный текст строки в одинарных кавычках */
<SINGLE_QUOTED_STRING>[^'\\\r\n]+ {
    stringBuffer.append(yytext, yyleng);
}

 /* Escape-последовательность звукового сигнала */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\a {
    stringBuffer.push_back('\a');
}

 /* Escape-последовательность удаления предыдущего символа */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\b {
    stringBuffer.push_back('\b');
}

 /* Escape-последовательность перевода страницы */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\f {
    stringBuffer.push_back('\f');
}

 /* Escape-последовательность перевода строки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\n {
    stringBuffer.push_back('\n');
}

 /* Escape-последовательность возврата каретки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\r {
    stringBuffer.push_back('\r');
}

 /* Escape-последовательность горизонтальной табуляции */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\t {
    stringBuffer.push_back('\t');
}

 /* Escape-последовательность вертикальной табуляции */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\v {
    stringBuffer.push_back('\v');
}

 /* Экранированная обратная косая черта */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\\ {
    stringBuffer.push_back('\\');
}

 /* Экранированная двойная кавычка */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\" {
    stringBuffer.push_back('"');
}

 /* Экранированная одинарная кавычка */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\' {
    stringBuffer.push_back('\'');
}

 /* Корректная шестнадцатеричная escape-последовательность */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x[0-9A-Fa-f]{2} {
    int high = hexDigitValue(yytext[2]);
    int low = hexDigitValue(yytext[3]);
    stringBuffer.push_back(static_cast<char>((high << 4) | low));
}

 /* Шестнадцатеричная escape-последовательность с неверным символом */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\x([0-9A-Fa-f]?[^0-9A-Fa-f"'\r\n][^"'\r\n]*)? {
    std::cout << "Error: invalid hexadecimal escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Неполная шестнадцатеричная escape-последовательность */
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

 /* Корректная Unicode escape-последовательность */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]+\} {
    errno = 0;
    unsigned long long value = std::strtoull(yytext + 3, nullptr, 16);

    if (errno == ERANGE || value >= 0x80000000ULL) {
        std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
        hasStringError = true;
    } else {
        appendUtf8(value);
    }
}

 /* Unicode escape-последовательность с неверным символом */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]*[^0-9A-Fa-f}"'\r\n][^"'\r\n]* {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Unicode escape-последовательность без шестнадцатеричных цифр */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]*\} {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Unicode escape-последовательность без закрывающей скобки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u\{[0-9A-Fa-f]* {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Unicode escape-последовательность без открывающей скобки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\u {
    std::cout << "Error: invalid Unicode escape: " << yytext << "\n";
    hasStringError = true;
}

 /* Escape-последовательность для пропуска пробелов */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\z[ \t\r\n\f\v]* {
}

 /* Экранированные возврат каретки и перевод строки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\r\n {
    stringBuffer.push_back('\n');
}

 /* Экранированные перевод строки и возврат каретки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\n\r {
    stringBuffer.push_back('\n');
}

 /* Экранированный возврат каретки внутри строки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\r {
    stringBuffer.push_back('\n');
}

 /* Экранированный перевод строки внутри строки */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\\n {
    stringBuffer.push_back('\n');
}

 /* Неизвестная escape-последовательность */
<DOUBLE_QUOTED_STRING,SINGLE_QUOTED_STRING>\\. {
    std::cout << "Error: invalid string escape: " << yytext << "\n";
    hasStringError = true;
    stringBuffer.push_back(yytext[1]);
}

 /* Незакрытая строка в двойных кавычках */
<DOUBLE_QUOTED_STRING>\r\n|\n\r|\r|\n {
    std::cout << "Error: unterminated double-quoted string\n";
    BEGIN(INITIAL);
}

 /* Незакрытая строка в одинарных кавычках */
<SINGLE_QUOTED_STRING>\r\n|\n\r|\r|\n {
    std::cout << "Error: unterminated single-quoted string\n";
    BEGIN(INITIAL);
}

 /* Незакрытая строка в двойных кавычках до конца файла */
<DOUBLE_QUOTED_STRING><<EOF>> {
    std::cout << "Error: unterminated double-quoted string before end of file\n";
    BEGIN(INITIAL);
}

 /* Незакрытая строка в одинарных кавычках до конца файла */
<SINGLE_QUOTED_STRING><<EOF>> {
    std::cout << "Error: unterminated single-quoted string before end of file\n";
    BEGIN(INITIAL);
}

 /* Закрытие строки в двойных кавычках */
<DOUBLE_QUOTED_STRING>\" {
    if (!hasStringError) {
        std::cout << "Found string: " << stringBuffer << "\n";
    }
    BEGIN(INITIAL);
}

 /* Закрытие строки в одинарных кавычках */
<SINGLE_QUOTED_STRING>\' {
    if (!hasStringError) {
        std::cout << "Found string: " << stringBuffer << "\n";
    }
    BEGIN(INITIAL);
}

 /* Операторы и разделители */
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
 /* Идентификатор */
{NAME_START}{NAME_SYMBOL}* {
    std::cout << "Found identifier: " << yytext << "\n";
}

 /* Игнорируемые пробельные символы */
{WHITESPACES}    ;

 /* Неизвестный символ */
.|\n        { std::cout << "Error: unknown token: " << yytext << "\n"; }

%%
