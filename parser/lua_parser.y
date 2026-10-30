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
    bool booleanValue;
    char* textValue;
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
%precedence EXPR_END            /* Конец выражения в списке или операторе */
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
%left '(' STRING '{' '[' '.' ':'        /* Продолжение цепочки выражения */

%start block /* Lua-файл */
%%

/* Содержимое файла, функции, условия или цикла */
block: st_list_em finish_st                             /* Операторы и завершающий return */
;

/* Один оператор Lua */
st: ';'                                                /* Пустой оператор */
|   BREAK                                              /* Выход из цикла */
|   FUNCTION func_name '(' par_list_em ')' block END   /* Объявление функции */
|   LOCAL FUNCTION ID '(' par_list_em ')' block END    /* Локальная функция */
|   GLOBAL FUNCTION ID '(' par_list_em ')' block END   /* Глобальная функция */
|   LOCAL att_name_list                                /* Локальные имена */
|   LOCAL att_name_list '=' expr_list                  /* Локальные имена со значениями */
|   GLOBAL att_name_list                               /* Глобальные имена */
|   GLOBAL att_name_list '=' expr_list                 /* Глобальные имена со значениями */
|   GLOBAL attrib_em '*'                               /* Все имена глобальные */
|   expr_list '=' expr_list                            /* Присваивание */
|   expr %prec EXPR_END                                /* Выражение-оператор */
|   LABEL_SEP ID LABEL_SEP                             /* Метка */
|   GOTO ID                                            /* Переход к метке */
|   DO block END                                       /* Блок do */
|   if_st                                               /* Условный оператор */
|   for_st                                              /* Цикл for */
|   while_st                                            /* Цикл while */
|   repeat_st                                           /* Цикл repeat */
;

/* Непустой список операторов */
st_list: st                                             /* Один оператор */
|   st_list st                                         /* Список и следующий оператор */
;

/* Список операторов, который может быть пустым */
st_list_em: %empty                                      /* Пустой список операторов */
|   st_list                                            /* Непустой список операторов */
;

/* Условный оператор if */
if_st: IF expr THEN block END                          /* if */
|   IF expr THEN block ELSE block END                  /* if ... else */
|   IF expr THEN block elseif_st_list END              /* if ... elseif */
|   IF expr THEN block elseif_st_list ELSE block END   /* if ... elseif ... else */
;

/* Одна или несколько веток elseif */
elseif_st_list: ELSEIF expr THEN block                 /* Одна ветка elseif */
|   elseif_st_list ELSEIF expr THEN block              /* Несколько веток elseif */
;

/* Цикл for */
for_st: FOR ID '=' expr ',' expr DO block END          /* Числовой for без шага */
|   FOR ID '=' expr ',' expr ',' expr DO block END     /* Числовой for с шагом */
|   FOR name_list IN expr_list DO block END            /* Обобщённый for */
;

/* Цикл while */
while_st: WHILE expr DO block END
;

/* Цикл repeat */
repeat_st:
     REPEAT block UNTIL expr %prec EXPR_END
;

/* Имя объявляемой функции */
func_name: dotted_name                                 /* f или a.b */
|   dotted_name ':' ID                                 /* a.b:method */
;

/* Имя из частей, разделённых точкой */
dotted_name: ID                                        /* Первое имя */
|   dotted_name '.' ID                                 /* Следующее поле */
;

/* Необязательный return в конце блока */
finish_st: %empty                                      /* Без return */
|   RETURN                                             /* return */
|   RETURN ';'                                         /* return с ; */
|   RETURN expr_list                                   /* return значений */
|   RETURN expr_list ';'                               /* return значений с ; */
;

/* Одно или несколько имён через запятую */
name_list: ID                                          /* Одно имя */
|   name_list ',' ID                                   /* Несколько имён */
;

/* Одно или несколько имён с атрибутами */
att_name_list: attrib_em ID attrib_em                  /* Первое имя */
|   att_name_list ',' ID attrib_em                     /* Следующее имя */
;

/* Атрибут, который может отсутствовать */
attrib_em: %empty
|   '<' ID '>'
;

/* Аргументы вызова функции */
args: '(' expr_list_em ')'                             /* Аргументы в скобках */
|   STRING                                             /* f "text" */
|   '{' field_list_em '}'                              /* f { key = value } */
;

/* Одно или несколько выражений через запятую */
expr_list:
     expr %prec EXPR_END                                /* Одно выражение */
|   expr_list ',' expr %prec EXPR_END                  /* Несколько выражений */
;

/* Список выражений, который может быть пустым */
expr_list_em: %empty                                   /* Пустой список */
|   expr_list                                          /* Непустой список */
;

/* Список параметров функции, который может быть пустым */
par_list_em: %empty                                    /* Без параметров */
|   par_list                                           /* С параметрами */
;

/* Именованные параметры и vararg */
par_list: name_list                                    /* Именованные параметры */
|   name_list ',' VARARG                               /* Имена и ... */
|   name_list ',' VARARG ID                            /* Имена и ... Name */
|   VARARG                                             /* ... */
|   VARARG ID                                          /* ... Name */
;

/* Вот тут начинаются наши классы - в луа таблицы, glhf*/

/* Список полей таблицы, который может быть пустым */
field_list_em: %empty                                  /* Пустая таблица */
|   field_list field_sep_end                           /* Таблица с полями */
;

/* Непустой список полей таблицы */
field_list: field                                      /* Первое поле */
|   field_list field_sep field                         /* Следующее поле */
;

/* Одно поле таблицы */
field: '[' expr ']' '=' expr                           /* [key] = value */
|   ID '=' expr                                        /* name = value */
|   expr                                               /* Элемент массива */
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
expr: INT                                               /* Целое число */
|   FLOAT                                              /* Вещественное число */
|   STRING                                             /* Строка */
|   BOOLEAN                                            /* true или false */
|   NIL                                                /* nil */
|   VARARG                                             /* ... */
|   ID                                                 /* Имя */
|   '(' expr ')'                                       /* Выражение в скобках */
|   expr '[' expr ']'                                  /* Обращение по индексу */
|   expr '.' ID                                        /* Обращение к полю */
|   expr args                                          /* Вызов функции */
|   expr ':' ID args                                   /* Вызов метода */
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

// first level object 
