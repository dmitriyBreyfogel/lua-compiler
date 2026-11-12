%{
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "ast/ParseResult.h"

int yylex();

void yyerror(const char* message)
{
    std::cerr << "Syntax error: " << message << "\n";
}

namespace {

using lua::ast::Node;
using lua::ast::NodeType;

Node* parserNode(NodeType type, std::string value = {})
{
    return lua::ast::makeNode(type, std::move(value)).release();
}

#define makeNode parserNode

Node* makeList()
{
    return makeNode(NodeType::List);
}

Node* append(Node* list, Node* node)
{
    list->addChild("item[" + std::to_string(list->children().size()) + "]", node);
    return list;
}

Node* add(Node* parent, const char* role, Node* child)
{
    if (child != nullptr) {
        parent->addChild(role, child);
    }
    return parent;
}

std::string takeText(char* text)
{
    std::string value(text);
    std::free(text);
    return value;
}

}
%}

%code requires {
#include "ast/Ast.h"
}

%define parse.error detailed
%define parse.lac full
%define lr.type ielr  

/* Возможные типы значений */
%union {
    long long integerValue;
    double floatValue;
    bool booleanValue;
    char* textValue;
    lua::ast::Node* nodeValue;
}

/* Числовые и текстовые токены с сем. значениями */
%token <integerValue> INT
%token <floatValue> FLOAT
%token <booleanValue> BOOLEAN
%token <textValue> ID STRING

/* Константы Lua */

%token NIL

/* Ключевые слова операторов и объявлений */

%token IF THEN ELSEIF ELSE END
%token WHILE REPEAT UNTIL FOR IN DO BREAK
%token FUNCTION RETURN
%token LOCAL GLOBAL
%token GOTO LABEL_SEP

/* Логические операторы и многоточие vararg */

%token AND OR NOT
%token VARARG

/* Операторы из нескольких символов */

%token CONCAT INT_DIV LSHIFT RSHIFT
%token EQ NE LE GE

/* Токен, которым лексер сообщает о своей ошибке */

%token INVALID

/* Приоритеты и ассоциативность операторов */
%precedence CALL_END            /* Завершение основы перед вызовом */
%left OR                       /* or */
%left AND                      /* and */
%left '<' '>' LE GE EQ NE      /* Операторы сравнения */
%left '|'                      /* Побитовое или */
%left '~'                      /* Побитовое исключающее или */
%left '&'                      /* Побитовое и */
%left LSHIFT RSHIFT            /* Сдвиги */
%right CONCAT                  /* Склейка строк: a .. (b .. c) */
%left '+' '-'                  /* Сложение и вычитание */
%left '*' '/' INT_DIV '%'      /* Умножение, деление, остаток */
%right UNARY                   /* Унарные -, not, # и ~ */
%right '^'                     /* Степень: a ^ (b ^ c) */
%left '('                                /* Продолжение вызова f()() */

%start block /* Lua-файл */

%type <nodeValue> block st st_list st_list_em finish_st
%type <nodeValue> if_st elseif_st_list for_st while_st repeat_st
%type <nodeValue> func_name dotted_name name_list att_name_list attrib_em
%type <nodeValue> args variable variable_list function_call expr expr_list expr_list_em
%type <nodeValue> par_list par_list_em field field_list field_list_em
%%

/* Содержимое файла, функции, условия или цикла */
block: st_list_em finish_st {
    $$ = add(makeNode(NodeType::Block), "statements", $1);
    add($$, "return", $2);
    lua::ast::setParsedBlock($$);
}
;

/* Один оператор Lua */
st: ';' { $$ = makeNode(NodeType::EmptyStatement); }
|   BREAK { $$ = makeNode(NodeType::BreakStatement); }
|   FUNCTION func_name '(' par_list_em ')' block END {
        $$ = add(makeNode(NodeType::FunctionDeclaration, "global"), "name", $2);
        add($$, "parameters", $4); add($$, "body", $6);
    }
|   LOCAL FUNCTION ID '(' par_list_em ')' block END {
        $$ = add(makeNode(NodeType::FunctionDeclaration, "local"), "name", makeNode(NodeType::Identifier, takeText($3)));
        add($$, "parameters", $5); add($$, "body", $7);
    }
|   GLOBAL FUNCTION ID '(' par_list_em ')' block END {
        $$ = add(makeNode(NodeType::FunctionDeclaration, "global"), "name", makeNode(NodeType::Identifier, takeText($3)));
        add($$, "parameters", $5); add($$, "body", $7);
    }
|   LOCAL att_name_list { $$ = add(makeNode(NodeType::LocalDeclaration), "names", $2); }
|   LOCAL att_name_list '=' expr_list {
        $$ = add(makeNode(NodeType::LocalDeclaration), "names", $2); add($$, "values", $4);
    }
|   GLOBAL att_name_list { $$ = add(makeNode(NodeType::GlobalDeclaration), "names", $2); }
|   GLOBAL att_name_list '=' expr_list {
        $$ = add(makeNode(NodeType::GlobalDeclaration), "names", $2); add($$, "values", $4);
    }
|   GLOBAL attrib_em '*' { $$ = add(makeNode(NodeType::GlobalAllDeclaration), "attribute", $2); }
|   variable_list '=' expr_list {
        $$ = add(makeNode(NodeType::AssignmentStatement), "targets", $1); add($$, "values", $3);
    }
|   function_call %prec CALL_END { $$ = add(makeNode(NodeType::CallStatement), "call", $1); }
|   LABEL_SEP ID LABEL_SEP { $$ = makeNode(NodeType::LabelStatement, takeText($2)); }
|   GOTO ID { $$ = makeNode(NodeType::GotoStatement, takeText($2)); }
|   DO block END { $$ = $2; }
|   if_st { $$ = $1; }
|   for_st { $$ = $1; }
|   while_st { $$ = $1; }
|   repeat_st { $$ = $1; }
;

/* Непустой список операторов */
st_list: st { $$ = append(makeList(), $1); }
|   st_list st { $$ = append($1, $2); }
;

/* Список операторов, который может быть пустым */
st_list_em: %empty { $$ = makeList(); }
|   st_list { $$ = $1; }
;

/* Условный оператор if */
if_st: IF expr THEN block END {
        $$ = add(makeNode(NodeType::IfStatement), "condition", $2); add($$, "then", $4);
    }
|   IF expr THEN block ELSE block END {
        $$ = add(makeNode(NodeType::IfStatement), "condition", $2); add($$, "then", $4); add($$, "else", $6);
    }
|   IF expr THEN block elseif_st_list END {
        $$ = add(makeNode(NodeType::IfStatement), "condition", $2); add($$, "then", $4); add($$, "elseif", $5);
    }
|   IF expr THEN block elseif_st_list ELSE block END {
        $$ = add(makeNode(NodeType::IfStatement), "condition", $2); add($$, "then", $4); add($$, "elseif", $5); add($$, "else", $7);
    }
;

/* Одна или несколько веток elseif */
elseif_st_list: ELSEIF expr THEN block {
        Node* branch = add(makeNode(NodeType::ElseIfBranch), "condition", $2);
        add(branch, "body", $4); $$ = append(makeList(), branch);
    }
|   elseif_st_list ELSEIF expr THEN block {
        Node* branch = add(makeNode(NodeType::ElseIfBranch), "condition", $3);
        add(branch, "body", $5); $$ = append($1, branch);
    }
;

/* Цикл for */
for_st: FOR ID '=' expr ',' expr DO block END {
        $$ = add(makeNode(NodeType::NumericForStatement), "variable", makeNode(NodeType::Identifier, takeText($2)));
        add($$, "start", $4); add($$, "finish", $6); add($$, "body", $8);
    }
|   FOR ID '=' expr ',' expr ',' expr DO block END {
        $$ = add(makeNode(NodeType::NumericForStatement), "variable", makeNode(NodeType::Identifier, takeText($2)));
        add($$, "start", $4); add($$, "finish", $6); add($$, "step", $8); add($$, "body", $10);
    }
|   FOR name_list IN expr_list DO block END {
        $$ = add(makeNode(NodeType::GenericForStatement), "names", $2);
        add($$, "iterators", $4); add($$, "body", $6);
    }
;

/* Цикл while */
while_st: WHILE expr DO block END {
    $$ = add(makeNode(NodeType::WhileStatement), "condition", $2); add($$, "body", $4);
}
;

/* Цикл repeat */
repeat_st: REPEAT block UNTIL expr {
    $$ = add(makeNode(NodeType::RepeatStatement), "body", $2); add($$, "condition", $4);
}
;

/* Имя объявляемой функции */
func_name: dotted_name { $$ = $1; }
|   dotted_name ':' ID {
    $$ = add(makeNode(NodeType::FieldExpression), "object", $1);
    add($$, "field", makeNode(NodeType::Identifier, takeText($3)));
}
;

/* Имя из частей, разделённых точкой */
dotted_name: ID { $$ = makeNode(NodeType::Identifier, takeText($1)); }
|   dotted_name '.' ID {
    $$ = add(makeNode(NodeType::FieldExpression), "object", $1);
    add($$, "field", makeNode(NodeType::Identifier, takeText($3)));
}
;

/* Необязательный return в конце блока */
finish_st: %empty { $$ = nullptr; }
|   RETURN { $$ = makeNode(NodeType::ReturnStatement); }
|   RETURN ';' { $$ = makeNode(NodeType::ReturnStatement); }
|   RETURN expr_list { $$ = add(makeNode(NodeType::ReturnStatement), "values", $2); }
|   RETURN expr_list ';' { $$ = add(makeNode(NodeType::ReturnStatement), "values", $2); }
;

/* Одно или несколько имён через запятую */
name_list: ID { $$ = append(makeList(), makeNode(NodeType::Identifier, takeText($1))); }
|   name_list ',' ID { $$ = append($1, makeNode(NodeType::Identifier, takeText($3))); }
;

/* Одно или несколько имён с атрибутами */
att_name_list: attrib_em ID attrib_em {
        Node* name = add(makeNode(NodeType::DeclarationName, takeText($2)), "before", $1);
        add(name, "after", $3); $$ = append(makeList(), name);
    }
|   att_name_list ',' ID attrib_em {
        Node* name = add(makeNode(NodeType::DeclarationName, takeText($3)), "attribute", $4);
        $$ = append($1, name);
    }
;

/* Атрибут, который может отсутствовать */
attrib_em: %empty { $$ = nullptr; }
|   '<' ID '>' { $$ = makeNode(NodeType::Attribute, takeText($2)); }
;

/* Аргументы вызова функции */
args: '(' expr_list_em ')' { $$ = $2; }
|   STRING { $$ = append(makeList(), makeNode(NodeType::StringLiteral, takeText($1))); }
|   '{' field_list_em '}' { $$ = append(makeList(), add(makeNode(NodeType::TableConstructor), "fields", $2)); }
;

/* Имя, поле таблицы или элемент по индексу */
variable: ID { $$ = makeNode(NodeType::Identifier, takeText($1)); }
|   variable '[' expr ']' { $$ = add(makeNode(NodeType::IndexExpression), "object", $1); add($$, "index", $3); }
|   variable '.' ID { $$ = add(makeNode(NodeType::FieldExpression), "object", $1); add($$, "field", makeNode(NodeType::Identifier, takeText($3))); }
|   function_call '[' expr ']' { $$ = add(makeNode(NodeType::IndexExpression), "object", $1); add($$, "index", $3); }
|   function_call '.' ID { $$ = add(makeNode(NodeType::FieldExpression), "object", $1); add($$, "field", makeNode(NodeType::Identifier, takeText($3))); }
|   '(' expr ')' '[' expr ']' { $$ = add(makeNode(NodeType::IndexExpression), "object", $2); add($$, "index", $5); }
|   '(' expr ')' '.' ID { $$ = add(makeNode(NodeType::FieldExpression), "object", $2); add($$, "field", makeNode(NodeType::Identifier, takeText($5))); }
;

/* Одна или несколько переменных в присваивании */
variable_list: variable { $$ = append(makeList(), $1); }
|   variable_list ',' variable { $$ = append($1, $3); }
;

/* Вызов функции или метода */
function_call: variable args { $$ = add(makeNode(NodeType::CallExpression), "function", $1); add($$, "arguments", $2); }
|   variable ':' ID args { $$ = add(makeNode(NodeType::MethodCallExpression, takeText($3)), "object", $1); add($$, "arguments", $4); }
|   function_call args { $$ = add(makeNode(NodeType::CallExpression), "function", $1); add($$, "arguments", $2); }
|   function_call ':' ID args { $$ = add(makeNode(NodeType::MethodCallExpression, takeText($3)), "object", $1); add($$, "arguments", $4); }
|   '(' expr ')' args { $$ = add(makeNode(NodeType::CallExpression), "function", $2); add($$, "arguments", $4); }
|   '(' expr ')' ':' ID args { $$ = add(makeNode(NodeType::MethodCallExpression, takeText($5)), "object", $2); add($$, "arguments", $6); }
;

/* Одно или несколько выражений через запятую */
expr_list: expr { $$ = append(makeList(), $1); }
|   expr_list ',' expr { $$ = append($1, $3); }
;

/* Список выражений, который может быть пустым */
expr_list_em: %empty { $$ = makeList(); }
|   expr_list { $$ = $1; }
;

/* Список параметров функции, который может быть пустым */
par_list_em: %empty { $$ = makeList(); }
|   par_list { $$ = $1; }
;

/* Именованные параметры и vararg */
par_list: name_list { $$ = $1; }
|   name_list ',' VARARG { $$ = append($1, makeNode(NodeType::VarargExpression)); }
|   name_list ',' VARARG ID { $$ = append($1, makeNode(NodeType::VarargExpression, takeText($4))); }
|   VARARG { $$ = append(makeList(), makeNode(NodeType::VarargExpression)); }
|   VARARG ID { $$ = append(makeList(), makeNode(NodeType::VarargExpression, takeText($2))); }
;

/* Вот тут начинаются наши классы - в луа таблицы, glhf*/

/* Список полей таблицы, который может быть пустым */
field_list_em: %empty { $$ = makeList(); }
|   field_list field_sep_end { $$ = $1; }
;

/* Непустой список полей таблицы */
field_list: field { $$ = append(makeList(), $1); }
|   field_list field_sep field { $$ = append($1, $3); }
;

/* Одно поле таблицы */
field: '[' expr ']' '=' expr { $$ = add(makeNode(NodeType::TableField), "key", $2); add($$, "value", $5); }
|   ID '=' expr { $$ = add(makeNode(NodeType::TableField), "key", makeNode(NodeType::StringLiteral, takeText($1))); add($$, "value", $3); }
|   expr { $$ = add(makeNode(NodeType::TableField), "value", $1); }
;

/* Запятая или точка с запятой между полями таблицы */
field_sep: ','                                         /* Разделитель полей */
|   ';'                                                /* Разделитель полей */
;

/* Необязательный разделитель после последнего поля */
field_sep_end: %empty
|   field_sep
;

/* Любое выражение Lua */
expr: INT { $$ = makeNode(NodeType::IntegerLiteral, std::to_string($1)); }
| FLOAT { $$ = makeNode(NodeType::FloatLiteral, std::to_string($1)); }
| STRING { $$ = makeNode(NodeType::StringLiteral, takeText($1)); }
| BOOLEAN { $$ = makeNode(NodeType::BooleanLiteral, $1 ? "true" : "false"); }
| NIL { $$ = makeNode(NodeType::NilLiteral); }
| VARARG { $$ = makeNode(NodeType::VarargExpression); }
| variable %prec CALL_END { $$ = $1; }
| function_call %prec CALL_END { $$ = $1; }
| '(' expr ')' %prec CALL_END { $$ = $2; }
| '{' field_list_em '}' { $$ = add(makeNode(NodeType::TableConstructor), "fields", $2); }
| FUNCTION '(' par_list_em ')' block END { $$ = add(makeNode(NodeType::FunctionExpression), "parameters", $3); add($$, "body", $5); }
| expr '+' expr { $$ = add(makeNode(NodeType::BinaryExpression, "+"), "left", $1); add($$, "right", $3); }
| expr '-' expr { $$ = add(makeNode(NodeType::BinaryExpression, "-"), "left", $1); add($$, "right", $3); }
| expr '*' expr { $$ = add(makeNode(NodeType::BinaryExpression, "*"), "left", $1); add($$, "right", $3); }
| expr '/' expr { $$ = add(makeNode(NodeType::BinaryExpression, "/"), "left", $1); add($$, "right", $3); }
| expr INT_DIV expr { $$ = add(makeNode(NodeType::BinaryExpression, "//"), "left", $1); add($$, "right", $3); }
| expr '%' expr { $$ = add(makeNode(NodeType::BinaryExpression, "%"), "left", $1); add($$, "right", $3); }
| expr '^' expr { $$ = add(makeNode(NodeType::BinaryExpression, "^"), "left", $1); add($$, "right", $3); }
| expr CONCAT expr { $$ = add(makeNode(NodeType::BinaryExpression, ".."), "left", $1); add($$, "right", $3); }
| expr '<' expr { $$ = add(makeNode(NodeType::BinaryExpression, "<"), "left", $1); add($$, "right", $3); }
| expr '>' expr { $$ = add(makeNode(NodeType::BinaryExpression, ">"), "left", $1); add($$, "right", $3); }
| expr LE expr { $$ = add(makeNode(NodeType::BinaryExpression, "<="), "left", $1); add($$, "right", $3); }
| expr GE expr { $$ = add(makeNode(NodeType::BinaryExpression, ">="), "left", $1); add($$, "right", $3); }
| expr EQ expr { $$ = add(makeNode(NodeType::BinaryExpression, "=="), "left", $1); add($$, "right", $3); }
| expr NE expr { $$ = add(makeNode(NodeType::BinaryExpression, "~="), "left", $1); add($$, "right", $3); }
| expr '&' expr { $$ = add(makeNode(NodeType::BinaryExpression, "&"), "left", $1); add($$, "right", $3); }
| expr '~' expr { $$ = add(makeNode(NodeType::BinaryExpression, "~"), "left", $1); add($$, "right", $3); }
| expr '|' expr { $$ = add(makeNode(NodeType::BinaryExpression, "|"), "left", $1); add($$, "right", $3); }
| expr LSHIFT expr { $$ = add(makeNode(NodeType::BinaryExpression, "<<"), "left", $1); add($$, "right", $3); }
| expr RSHIFT expr { $$ = add(makeNode(NodeType::BinaryExpression, ">>"), "left", $1); add($$, "right", $3); }
| expr AND expr { $$ = add(makeNode(NodeType::BinaryExpression, "and"), "left", $1); add($$, "right", $3); }
| expr OR expr { $$ = add(makeNode(NodeType::BinaryExpression, "or"), "left", $1); add($$, "right", $3); }
| '-' expr %prec UNARY { $$ = add(makeNode(NodeType::UnaryExpression, "-"), "operand", $2); }
| NOT expr %prec UNARY { $$ = add(makeNode(NodeType::UnaryExpression, "not"), "operand", $2); }
| '#' expr %prec UNARY { $$ = add(makeNode(NodeType::UnaryExpression, "#"), "operand", $2); }
| '~' expr %prec UNARY { $$ = add(makeNode(NodeType::UnaryExpression, "~"), "operand", $2); }
;

%%

// first level object 
