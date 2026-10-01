%{
#include <iostream>

int yylex();

void yyerror(const char* message)
{
    std::cerr << "Syntax error: " << message << "\n";
}
%}

%define parse.error detailed
%define parse.lac full
%define lr.type ielr
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

%left TOKEN_OR
%left TOKEN_AND
%nonassoc '<' '>' TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL TOKEN_EQUAL TOKEN_NOT_EQUAL
%left '|'
%left '~'
%left '&'
%left TOKEN_SHIFT_LEFT TOKEN_SHIFT_RIGHT
%right TOKEN_CONCAT
%left '+' '-'
%left '*' '/' TOKEN_INT_DIV '%'
%precedence TOKEN_UNARY
%right '^'

%start chunk

%%

chunk:
    block
;

block:
    st_list_em finish_st
;

st_list_em:
    %empty
|   st_list
;

st_list:
    st
|   st_list st
;

st:
    ';'
|   TOKEN_LOCAL name_list
|   TOKEN_LOCAL name_list '=' expr_list
|   variable_list '=' expr_list
|   function_call
;

finish_st:
    %empty
|   TOKEN_BREAK
|   TOKEN_RETURN
|   TOKEN_RETURN ';'
|   TOKEN_RETURN expr_list
|   TOKEN_RETURN expr_list ';'
;

name_list:
    TOKEN_IDENTIFIER
|   name_list ',' TOKEN_IDENTIFIER
;

variable_list:
    variable
|   variable_list ',' variable
;

variable:
    TOKEN_IDENTIFIER
|   variable '[' expr ']'
|   variable '.' TOKEN_IDENTIFIER
|   function_call '[' expr ']'
|   function_call '.' TOKEN_IDENTIFIER
;

prefix_expr:
    variable
|   function_call
|   '(' expr ')'
;

function_call:
    variable args
|   variable ':' TOKEN_IDENTIFIER args
|   function_call args
|   function_call ':' TOKEN_IDENTIFIER args
;

args:
    '(' expr_list_em ')'
;

expr_list:
    expr
|   expr_list ',' expr
;

expr_list_em:
    %empty
|   expr_list
;

expr:
    TOKEN_INTEGER
|   TOKEN_HEX_INTEGER
|   TOKEN_FLOAT
|   TOKEN_HEX_FLOAT
|   TOKEN_STRING
|   TOKEN_TRUE
|   TOKEN_FALSE
|   TOKEN_NIL
|   prefix_expr
|   expr '+' expr
|   expr '-' expr
|   expr '*' expr
|   expr '/' expr
|   expr TOKEN_INT_DIV expr
|   expr '%' expr
|   expr '^' expr
|   expr TOKEN_CONCAT expr
|   expr '<' expr
|   expr '>' expr
|   expr TOKEN_LESS_EQUAL expr
|   expr TOKEN_GREATER_EQUAL expr
|   expr TOKEN_EQUAL expr
|   expr TOKEN_NOT_EQUAL expr
|   expr '&' expr
|   expr '~' expr
|   expr '|' expr
|   expr TOKEN_SHIFT_LEFT expr
|   expr TOKEN_SHIFT_RIGHT expr
|   expr TOKEN_AND expr
|   expr TOKEN_OR expr
|   '-' expr %prec TOKEN_UNARY
|   TOKEN_NOT expr %prec TOKEN_UNARY
|   '#' expr %prec TOKEN_UNARY
|   '~' expr %prec TOKEN_UNARY
;

%%
