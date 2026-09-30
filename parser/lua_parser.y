%{
#include <iostream>

int yylex();

void yyerror(const char* message)
{
    std::cerr << "Syntax error: " << message << "\n";
}
%}

%define parse.error detailed
%union {
    long long integerValue;
    double floatValue;
    char* textValue;
}

%token <integerValue> TOKEN_INTEGER TOKEN_HEX_INTEGER
%token <floatValue> TOKEN_FLOAT TOKEN_HEX_FLOAT
%token <textValue> TOKEN_IDENTIFIER TOKEN_STRING

%token TOKEN_AND TOKEN_BREAK TOKEN_DO TOKEN_ELSE TOKEN_ELSEIF TOKEN_END
%token TOKEN_FALSE TOKEN_FOR TOKEN_FUNCTION TOKEN_GLOBAL TOKEN_GOTO TOKEN_IF
%token TOKEN_IN TOKEN_LOCAL TOKEN_NIL TOKEN_NOT TOKEN_OR TOKEN_REPEAT
%token TOKEN_RETURN TOKEN_THEN TOKEN_TRUE TOKEN_UNTIL TOKEN_WHILE

%token TOKEN_VARARG TOKEN_CONCAT TOKEN_INT_DIV TOKEN_SHIFT_LEFT TOKEN_SHIFT_RIGHT
%token TOKEN_EQUAL TOKEN_NOT_EQUAL TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL TOKEN_LABEL
%token TOKEN_ERROR

%start chunk

%%

chunk:
    statement_list
;

statement_list:
    %empty
|   statement_list statement
;

statement:
    ';'
|   TOKEN_LOCAL name_list
|   TOKEN_LOCAL name_list '=' expression_list
|   variable_list '=' expression_list
|   TOKEN_RETURN expression_list
;

name_list:
    TOKEN_IDENTIFIER
|   name_list ',' TOKEN_IDENTIFIER
;

variable_list:
    TOKEN_IDENTIFIER
|   variable_list ',' TOKEN_IDENTIFIER
;

expression_list:
    expression
|   expression_list ',' expression
;

expression:
    TOKEN_INTEGER
|   TOKEN_HEX_INTEGER
|   TOKEN_FLOAT
|   TOKEN_HEX_FLOAT
|   TOKEN_STRING
|   TOKEN_TRUE
|   TOKEN_FALSE
|   TOKEN_NIL
|   TOKEN_IDENTIFIER
|   '(' expression ')'
;

%%
