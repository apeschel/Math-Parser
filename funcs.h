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

#ifndef FUNCS_H
#define FUNCS_H
#include <glib.h>

extern int yylineno;
void yyerror(char *s, ...);

typedef float t_num;
typedef char t_var;

enum Position
{
    LEFT,
    RIGHT
};

struct Data
{
    enum NodeType
    {
        OPERATOR,
        NUMBER,
        VARIABLE
    } type;

    union
    {
        char oper;
        t_num number;
        struct
        {
            enum
            {
                POSITIVE,
                NEGATIVE
            } sign;
            t_num count;
            t_var var_name;
            t_num exponent;
        };
    };
};

GNode* newast(char op, const GNode* l, const GNode* r);
GNode* newnum(t_num d);
GNode* newvar(t_var c);

void print_tree(const GNode* t);
void combine_trees(GNode* child, GNode* parent);
void flatten_tree(GNode* t);

void simplify_tree_ops(GNode* t);
void simplify_op(GNode* t);
void invert(GNode* t);
void negate(GNode* t);

void reduce_tree(GNode* t);
void reduce(GNode* t);

#endif // FUNCS_H
