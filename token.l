%{
    #include <stdlib.h>
    #include "funcs.h"
    #include "parse.tab.h"
%}

%option noyywrap nodefault yylineno

%%

[A-Za-z]+       {
                    yylval.varval = *yytext;
                    return VAR;
                }

-?[0-9]+        {
                    yylval.numval = atoi(yytext);
                    return NUM;
                }

[-()*/+\n^]     return *yytext;

[ \t]+          /* ignore whitespace */;

.               yyerror("invalid character");

%%

