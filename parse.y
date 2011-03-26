%{
    #include <stdio.h>
    #include <math.h>
    #include "funcs.h"
    int yylex();
%}

%union
{
    struct ast  *t;
    char        varval;
    int         intval;
}

%type <varval> VAR;
%type <intval> NUM;
%type <t> expr

%token VAR NUM;

%left '-' '+'
%left '*' '/'
%right '^'

%%

statement:
         statement expr '\n'        {
                                        print_tree($2);
                                        printf("\n");
                                        free_tree($2);
                                    }
         |
         ;

expr:
         NUM                        { $$ = newnum($1); }
         | VAR                      { $$ = newvar($1); }
         | expr '+' expr            { $$ = newast('+', $1, $3); }
         | expr '-' expr            { $$ = newast('-', $1, $3); }
         | expr '*' expr            { $$ = newast('*', $1, $3); }
         | expr '/' expr            { $$ = newast('/', $1, $3); }
         | expr '^' expr            { $$ = newast('^', $1, $3); }
         | '(' expr ')'             { $$ = $2; }

%%

int main()
{
    yyparse();
    return 0;
}
