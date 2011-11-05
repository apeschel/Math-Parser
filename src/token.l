/*
 * Parser: Parses math expressions from infix to prefix form.
 * Copyright (C) 2011  Aaron Peschel
 * This file is part of Parser.
 *
 * Parser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parser.  If not, see <http://www.gnu.org/licenses/>.
 */

%{
    #include <stdlib.h>
    #include "funcs.h"
    #include "parse.tab.h"
%}

%option yylineno
%option noyywrap
%option nodefault
%option nounput
%option noinput

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

