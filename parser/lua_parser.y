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

/* Возможные типы значений */
%union {
    long long integerValue;
    double floatValue;
    char* textValue;
}

/* Числовые и текстовые токены с сем. значениями */
%token <integerValue> INT HEX_INT
%token <floatValue> FLOAT HEX_FLOAT
%token <textValue> ID STRING

/* Константы Lua */

%token TRUE FALSE NIL

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
%left OR                       /* or */
%left AND                      /* and */
%nonassoc '<' '>' LE GE EQ NE  /* Сравнения нельзя записывать цепочкой */
%left '|'                      /* Побитовое или */
%left '~'                      /* Побитовое исключающее или */
%left '&'                      /* Побитовое и */
%left LSHIFT RSHIFT            /* Сдвиги */
%right CONCAT                  /* Склейка строк: a .. (b .. c) */
%left '+' '-'                  /* Сложение и вычитание */
%left '*' '/' INT_DIV '%'      /* Умножение, деление, остаток */
%precedence UNARY              /* Унарные -, not, # и ~ */
%right '^'                     /* Степень: a ^ (b ^ c) */

%start block /* Lua-файл */

%%

/* Содержимое файла, функции, условия или цикла */
block:
    st_list_em finish_st                                /* Операторы и завершающий return */
;

/* Список операторов, который может быть пустым */
st_list_em:
    %empty                                             /* Пустой список операторов */
|   st_list                                            /* Непустой список операторов */
;

/* Непустой список операторов */
st_list:
    st                                                 /* Один оператор */
|   st_list st                                         /* Список и следующий оператор */
;

/* Один оператор Lua */
st:
    ';'                                                /* Пустой оператор */
|   BREAK                                              /* Выход из цикла */
|   LOCAL name_list                                    /* Локальные имена */
|   LOCAL name_list '=' expr_list                      /* Локальные имена со значениями */
|   FUNCTION func_name '(' par_list_em ')' block END   /* Объявление функции */
|   LOCAL FUNCTION ID '(' par_list_em ')' block END    /* Локальная функция */
|   variable_list '=' expr_list                        /* Присваивание */
|   function_call                                      /* Вызов функции */
|   if_st                                               /* Условный оператор */
|   for_st                                              /* Цикл for */
|   while_st                                            /* Цикл while */
|   repeat_st                                           /* Цикл repeat */
;

/* Условный оператор if */
if_st:
    IF expr THEN block END                             /* if */
|   IF expr THEN block ELSE block END                  /* if ... else */
|   IF expr THEN block elseif_st_list END              /* if ... elseif */
|   IF expr THEN block elseif_st_list ELSE block END   /* if ... elseif ... else */
;

/* Одна или несколько веток elseif */
elseif_st_list:
    elseif_st                                          /* Одна ветка elseif */
|   elseif_st_list elseif_st                           /* Несколько веток elseif */
;

/* Одна ветка elseif */
elseif_st:
    ELSEIF expr THEN block                             /* Условие и тело elseif */
;

/* Цикл for */
for_st:
    FOR ID '=' expr ',' expr DO block END              /* Числовой for без шага */
|   FOR ID '=' expr ',' expr ',' expr DO block END     /* Числовой for с шагом */
|   FOR name_list IN expr_list DO block END            /* Обобщённый for */
;

/* Цикл while */
while_st:
    WHILE expr DO block END
;

/* Цикл repeat */
repeat_st:
    REPEAT block UNTIL expr
;

/* Имя объявляемой функции */
func_name:
    dotted_name                                        /* f или a.b */
|   dotted_name ':' ID                                 /* a.b:method */
;

/* Имя из частей, разделённых точкой */
dotted_name:
    ID                                                 /* Первое имя */
|   dotted_name '.' ID                                 /* Следующее поле */
;

/* Необязательный return в конце блока */
finish_st:
    %empty                                             /* Без return */
|   RETURN                                             /* return */
|   RETURN ';'                                         /* return с ; */
|   RETURN expr_list                                   /* return значений */
|   RETURN expr_list ';'                               /* return значений с ; */
;

/* Одно или несколько имён через запятую */
name_list:
    ID                                                 /* Одно имя */
|   name_list ',' ID                                   /* Несколько имён */
;

/* Одна или несколько переменных в присваивании */
variable_list:
    variable                                           /* Одна переменная */
|   variable_list ',' variable                         /* Несколько переменных */
;

/* Переменная: имя, поле таблицы или элемент по индексу */
variable:
    ID                                                 /* Простая переменная */
|   variable '[' expr ']'                              /* table[key] */
|   variable '.' ID                                    /* table.name */
|   function_call '[' expr ']'                         /* f()[key] */
|   function_call '.' ID                               /* f().name */
;

/* Вызов функции или метода */
function_call:
    variable args                                      /* f(args) */
|   variable ':' ID args                                /* object:method(args) */
|   function_call args                                  /* f()() */
|   function_call ':' ID args                           /* f():method() */
;

/* Аргументы вызова функции */
args:
    '(' expr_list_em ')'                               /* Аргументы в скобках */
|   STRING                                             /* f "text" */
|   '{' field_list_em '}'                              /* f { key = value } */
;

/* Одно или несколько выражений через запятую */
expr_list:
    expr                                               /* Одно выражение */
|   expr_list ',' expr                                 /* Несколько выражений */
;

/* Список выражений, который может быть пустым */
expr_list_em:
    %empty                                             /* Пустой список */
|   expr_list                                          /* Непустой список */
;

/* Список параметров функции, который может быть пустым */
par_list_em:
    %empty                                             /* Без параметров */
|   par_list                                           /* С параметрами */
;

/* Именованные параметры и vararg */
par_list:
    name_list                                          /* Именованные параметры */
|   name_list ',' VARARG                               /* Имена и ... */
|   VARARG                                             /* Только ... */
;

/* Вот тут начинаются наши классы - в луа таблицы, glhf*/

/* Список полей таблицы, который может быть пустым */
field_list_em:
    %empty                                             /* Пустая таблица */
|   field_list                                         /* Таблица с полями */
;

/* Непустой список полей таблицы */
field_list:
    field                                              /* Первое поле */
|   field_list field_sep field                         /* Следующее поле */
|   field_list field_sep                               /* Разделитель после последнего поля */
;

/* Одно поле таблицы */
field:
    '[' expr ']' '=' expr                              /* [key] = value */
|   ID '=' expr                                        /* name = value */
|   expr                                               /* Элемент массива */
;

/* Запятая или точка с запятой между полями таблицы */
field_sep:
    ','                                                /* Разделитель полей */
|   ';'                                                /* Разделитель полей */
;

/* Любое выражение Lua */
expr:
    INT                                                /* Десятичное целое */
|   HEX_INT                                            /* Шестнадцатеричное целое */
|   FLOAT                                              /* Десятичное вещественное */
|   HEX_FLOAT                                          /* Шестнадцатеричное вещественное */
|   STRING                                             /* Строка */
|   TRUE                                               /* true */
|   FALSE                                              /* false */
|   NIL                                                /* nil */
|   VARARG                                             /* ... */
|   variable                                           /* Переменная */
|   function_call                                      /* Вызов функции */
|   '(' expr ')'                                       /* Выражение в скобках */
|   '{' field_list_em '}'                              /* Таблица */
|   FUNCTION '(' par_list_em ')' block END             /* Анонимная функция */
|   expr '+' expr                                      /* Сложение */
|   expr '-' expr                                      /* Вычитание */
|   expr '*' expr                                      /* Умножение */
|   expr '/' expr                                      /* Деление */
|   expr INT_DIV expr                                  /* Целочисленное деление */
|   expr '%' expr                                      /* Остаток от деления */
|   expr '^' expr                                      /* Степень */
|   expr CONCAT expr                                   /* Склейка строк */
|   expr '<' expr                                      /* Меньше */
|   expr '>' expr                                      /* Больше */
|   expr LE expr                                       /* Меньше или равно */
|   expr GE expr                                       /* Больше или равно */
|   expr EQ expr                                       /* Равно */
|   expr NE expr                                       /* Не равно */
|   expr '&' expr                                      /* Побитовое и */
|   expr '~' expr                                      /* Побитовое исключающее или */
|   expr '|' expr                                      /* Побитовое или */
|   expr LSHIFT expr                                   /* Сдвиг влево */
|   expr RSHIFT expr                                   /* Сдвиг вправо */
|   expr AND expr                                      /* Логическое и */
|   expr OR expr                                       /* Логическое или */
|   '-' expr %prec UNARY                               /* Унарный минус */
|   NOT expr %prec UNARY                               /* Логическое не */
|   '#' expr %prec UNARY                               /* Длина */
|   '~' expr %prec UNARY                               /* Унарное побитовое не */
;

%%
