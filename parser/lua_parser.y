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

%token <integerValue> INT HEX_INT
%token <floatValue> FLOAT HEX_FLOAT
%token <textValue> ID STRING

%token TRUE FALSE NIL
%token IF THEN ELSEIF ELSE END
%token WHILE REPEAT UNTIL FOR IN DO BREAK
%token FUNCTION RETURN
%token LOCAL GLOBAL
%token GOTO LABEL_SEP
%token AND OR NOT
%token VARARG
%token CONCAT INT_DIV LSHIFT RSHIFT
%token EQ NE LE GE
%token INVALID

%left OR
%left AND
%nonassoc '<' '>' LE GE EQ NE
%left '|'
%left '~'
%left '&'
%left LSHIFT RSHIFT
%right CONCAT
%left '+' '-'
%left '*' '/' INT_DIV '%'
%precedence UNARY
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
|   BREAK
|   LOCAL name_list
|   LOCAL name_list '=' expr_list
|   FUNCTION func_name '(' par_list_em ')' block END
|   LOCAL FUNCTION ID '(' par_list_em ')' block END
|   variable_list '=' expr_list
|   function_call
|   if_st
|   WHILE expr DO block END
|   REPEAT block UNTIL expr
|   FOR ID '=' expr ',' expr DO block END
|   FOR ID '=' expr ',' expr ',' expr DO block END
|   FOR name_list IN expr_list DO block END
;

if_st:
    IF expr THEN block END
|   IF expr THEN block ELSE block END
|   IF expr THEN block elseif_st_list END
|   IF expr THEN block elseif_st_list ELSE block END
;

elseif_st_list:
    elseif_st
|   elseif_st_list elseif_st
;

elseif_st:
    ELSEIF expr THEN block
;

func_name:
    dotted_name
|   dotted_name ':' ID
;

dotted_name:
    ID
|   dotted_name '.' ID
;

finish_st:
    %empty
|   RETURN
|   RETURN ';'
|   RETURN expr_list
|   RETURN expr_list ';'
;

name_list:
    ID
|   name_list ',' ID
;

variable_list:
    variable
|   variable_list ',' variable
;

variable:
    ID
|   variable '[' expr ']'
|   variable '.' ID
|   function_call '[' expr ']'
|   function_call '.' ID
;

prefix_expr:
    variable
|   function_call
|   '(' expr ')'
;

function_call:
    variable args
|   variable ':' ID args
|   function_call args
|   function_call ':' ID args
;

args:
    '(' expr_list_em ')'
|   STRING
|   table_constructor
;

expr_list:
    expr
|   expr_list ',' expr
;

expr_list_em:
    %empty
|   expr_list
;

par_list_em:
    %empty
|   par_list
;

par_list:
    name_list
|   name_list ',' VARARG
|   VARARG
;

table_constructor:
    '{' field_list_em '}'
;

field_list_em:
    %empty
|   field_list
;

field_list:
    field
|   field_list field_sep field
|   field_list field_sep
;

field:
    '[' expr ']' '=' expr
|   ID '=' expr
|   expr
;

field_sep:
    ','
|   ';'
;

expr:
    INT
|   HEX_INT
|   FLOAT
|   HEX_FLOAT
|   STRING
|   TRUE
|   FALSE
|   NIL
|   VARARG
|   prefix_expr
|   table_constructor
|   FUNCTION '(' par_list_em ')' block END
|   expr '+' expr
|   expr '-' expr
|   expr '*' expr
|   expr '/' expr
|   expr INT_DIV expr
|   expr '%' expr
|   expr '^' expr
|   expr CONCAT expr
|   expr '<' expr
|   expr '>' expr
|   expr LE expr
|   expr GE expr
|   expr EQ expr
|   expr NE expr
|   expr '&' expr
|   expr '~' expr
|   expr '|' expr
|   expr LSHIFT expr
|   expr RSHIFT expr
|   expr AND expr
|   expr OR expr
|   '-' expr %prec UNARY
|   NOT expr %prec UNARY
|   '#' expr %prec UNARY
|   '~' expr %prec UNARY
;

%%
