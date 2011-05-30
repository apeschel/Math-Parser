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
    #include <stdio.h>
    #include <math.h>
    #include <glib.h>
    #include "funcs.h"
    int yylex();
%}

%union
{
    GNode*   t;
    t_var    varval;
    t_num    numval;
}

%type <varval> VAR;
%type <numval> NUM;
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
                                        simplify_tree_ops($2);
                                        flatten_tree($2);
                                        reduce_tree($2);
                                        print_tree($2);
                                        printf("\n");
                                        g_node_destroy($2);
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
