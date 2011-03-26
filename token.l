%{
    #include <stdlib.h>
    #include "parse.tab.h"
    #include "funcs.h"
%}

%option noyywrap nodefault yylineno

%%

[A-Za-z]+       {
                    yylval.varval = *yytext;
                    return VAR;
                }

-?[0-9]+        {
                    yylval.intval = atoi(yytext);
                    return NUM;
                }

[-()*/+\n^]     return *yytext;

[ \t]+          /* ignore whitespace */;

.               yyerror("invalid character");

%%

