%{
    #include <stdio.h>
    #include <math.h>
    #include "funcs.h"
    int yylex();
%}

%union
{
    char    varval;
    int     intval;
}

%type <varval> VAR;
%type <intval> expr NUM;

%token VAR NUM;

%left '-' '+'
%left '*' '/'
%right '^'

%%

statement:
         statement expr '\n'        { printf("%d\n", $2); }
         |
         ;

expr:
         NUM                        { $$ = $1; }
         | expr '+' expr            { $$ = $1 + $3; }
         | expr '-' expr            { $$ = $1 - $3; }
         | expr '*' expr            { $$ = $1 * $3; }
         | expr '/' expr            { $$ = floor($1 / $3); }
         | expr '^' expr            { $$ = pow($1, $3); }
         | '(' expr ')'             { $$ = $2; }

%%

int main()
{
    yyparse();
    return 0;
}
