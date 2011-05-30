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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <glib.h>

#include "funcs.h"

void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

GNode* newast(char op, const GNode* l, const GNode* r)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = OPERATOR;
    d->oper = op;

    GNode* result = g_node_new(d);
    g_node_prepend(result, (GNode*) l);
    g_node_append(result, (GNode*) r);

    return result;
}

struct Data* new_num_data(t_num n)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = NUMBER;
    d->number = n;

    return d;
}

GNode* newnum(t_num n)
{
    struct Data* d = new_num_data(n);
    GNode* result = g_node_new(d);

    return result;
}

struct Data* new_var_data(t_var c)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = VARIABLE;
    d->count = 1;
    d->var_name = c;
    d->exponent = 1;

    return d;
}

GNode* newvar(t_var c)
{
    struct Data* d = new_var_data(c);
    GNode* result = g_node_new(d);

    return result;
}

void print_var(const struct Data* d)
{
    if (d->count == -1)
    {
        printf("-");
    }
    else if (d->count != 1)
    {
        printf("%f", d->count);
    }

    printf("%c", d->var_name);

    if (d->exponent != 1)
    {
        printf("^%f", d->exponent);
    }
}

void print_tree(const GNode* t)
{
    struct Data* d = t->data;
    // This check is here to prevent a space from appearing at the start of the line.
    if (!G_NODE_IS_ROOT(t))
    {
        printf(" ");
    }

    switch (d->type)
    {
        case NUMBER:
            printf("%f", d->number);
            break;

        case VARIABLE:
            print_var(d);
            break;

        case OPERATOR:
            printf("(%c", d->oper);
            g_node_children_foreach((GNode*) t, G_TRAVERSE_ALL, (GNodeForeachFunc) &print_tree, NULL);
            printf(")");
            break;

        default:
            fprintf(stderr, "\nInternal error: unrecognized bad node %c\n", d->type);
            exit(0);
    }
}

void combine_trees(GNode* child, GNode* parent)
{
    struct Data* child_d  = child->data;
    struct Data* parent_d = parent->data;

    if (G_NODE_IS_LEAF(parent) ||
        G_NODE_IS_LEAF(child))
    {
        return;
    }

    if (child_d->oper != parent_d->oper)
    {
        return;
    }

    // Only flatten operators with the communative property.
    switch (child_d->oper)
    {

        case '+':
        case '-':
        case '/':
        case '*':
            while (g_node_n_children(child) > 0)
            {
                GNode* grandchild = g_node_first_child(child);
                g_node_unlink(grandchild);
                g_node_insert_before(parent, child, grandchild);
            }

            g_node_destroy(child);
            return;

        default:
            return;
    }
}

void flatten_tree(GNode* t)
{
    if (G_NODE_IS_LEAF(t))
    {
        return;
    }

    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, (GNodeForeachFunc) &flatten_tree, NULL);
    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, (GNodeForeachFunc) &combine_trees, t);
}

void simplify_tree_ops(GNode* t)
{
    if (G_NODE_IS_LEAF(t))
    {
        return;
    }

    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, (GNodeForeachFunc) &simplify_tree_ops, NULL);
    simplify_op(t);
}

static void simplify_inv_op(GNode* t, void (*op_func)(GNode* t))
{
    if (g_node_n_children(t) != 2)
    {
        return;
    }

    GNode* right_child = g_node_nth_child(t, RIGHT);
    op_func(right_child);
}

void simplify_op(GNode* t)
{
    struct Data* d = t->data;

    if (d->type != OPERATOR)
    {
        return;
    }

    switch (d->oper)
    {
        case '-':
            simplify_inv_op(t, negate);
            d->oper = '+';
            return;

        case '/':
            simplify_inv_op(t, invert);
            d->oper = '*';
            return;

        default:
            return;
    }
}

void inversion_func
    (
        GNode* t,
        void (*num_operator)(struct Data* d),
        void (*var_operator)(struct Data* d)
    )
{
    struct Data* d = t->data;
    GNode* left_child = g_node_nth_child(t, LEFT);
    GNode* right_child = g_node_nth_child(t, RIGHT);

    switch(d->type)
    {
        case NUMBER:
            num_operator(d);
            return;

        case VARIABLE:
            var_operator(d);
            return;

        case OPERATOR:
            inversion_func(left_child, num_operator, var_operator);
            inversion_func(right_child, num_operator, var_operator);
            return;
    }
}

static void invert_num(struct Data* d)
{
    d->number = 1 / d->number;
}

static void invert_var(struct Data* d)
{
    d->exponent *= -1;
}

void invert(GNode* t)
{
    inversion_func(t, invert_num, invert_var);
}

static void negate_num(struct Data* d)
{
    d->number = -(d->number);
}

static void negate_var(struct Data* d)
{
    d->count *= -1;
}

void negate(GNode* t)
{
    inversion_func(t, negate_num, negate_var);
}

void reduce_tree(GNode* t)
{
    if (G_NODE_IS_LEAF(t))
    {
        return;
    }

    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, (GNodeForeachFunc) &reduce_tree, NULL);
    reduce(t);
}

static t_num add_op(t_num x, t_num y)
{
    return x + y;
}

static t_num mul_op(t_num x, t_num y)
{
    return x * y;
}

static void var_op(GNode* left, GNode* right, t_num (*num_op)(t_num x, t_num y))
{
    struct Data*  left_d = left->data;
    struct Data* right_d = right->data;

    if (left_d->var_name != right_d->var_name)
    {
        return;
    }
    else if (left_d->exponent != right_d->exponent)
    {
        return;
    }

    left_d->count = num_op(left_d->count, right_d->count);
    g_node_destroy(right);
}

static void mul_vars(GNode* left, GNode* right)
{
    var_op(left, right, mul_op);
}

static void add_vars(GNode* left, GNode* right)
{
    var_op(left, right, add_op);
}

static void foldr_reduce(GNode* t, t_num sum, t_num (*num_op)(t_num x, t_num y))
{
    GNode* child = t->children;
    GNode* next_child;

    // vars has a spot for every possible char value.
    // this is a kludge to use in place of a proper hash.
    GSList* vars[256] = { 0 };

    while (NULL != child)
    {
        struct Data* d = child->data;
        int index;
        switch (d->type)
        {
            case NUMBER:
                sum = num_op(sum, d->number);
                next_child = child->next;
                g_node_destroy(child);
                break;

            case VARIABLE:
                index = (int) d->var_name;
                vars[index] = g_slist_append(vars[index], child);
                next_child = child->next;
                break;

            case OPERATOR:
                // No further simplications can be made.
                next_child = child->next;
                break;
        }

        child = next_child;
    }

    // TODO: Add logic for simplifying variables.

    int i;
    for (i = 0; i < 256; i++)
    {
        g_slist_free(vars[i]);
    }

    if (g_node_n_children(t) == 0)
    {
        free(t->data);
        t->data = new_num_data(sum);
    }
    else
    {
        g_node_append(t, newnum(sum));
    }
}

void reduce_exp(GNode *t)
{
    GNode*  left_child = g_node_nth_child(t, LEFT);
    GNode* right_child = g_node_nth_child(t, RIGHT);

    struct Data* base_d = left_child->data;
    struct Data*  exp_d = right_child->data;

    if (base_d->type == NUMBER &&
         exp_d->type == NUMBER)
    {
        t_num val = pow(base_d->number, exp_d->number);

        free(t->data);
        t->data = new_num_data(val);
    }

    if (base_d->type == VARIABLE &&
         exp_d->type == NUMBER)
    {
        base_d->exponent *= exp_d->number;
        g_node_destroy(right_child);
    }
}

void reduce(GNode* t)
{
    struct Data* d = t->data;

    if (d->type != OPERATOR)
    {
        return;
    }

    switch(d->oper)
    {
        case '+':
            foldr_reduce(t, 0, &add_op);
            return;

        case '*':
            foldr_reduce(t, 1, &mul_op);
            return;

        case '^':
            reduce_exp(t);
            return;

        default:
            return;
    }
}

static int REDUCE_FLAG = FALSE;

void process_tree(GNode* t)
{
    if (REDUCE_FLAG)
    {
        flatten_tree(t);
        reduce_tree(t);
        print_tree(t);
        printf("\n");
    }
    else
    {
        print_tree(t);
        simplify_tree_ops(t);
        printf("\n");
    }

    g_node_destroy(t);
}

int main(int argc, char *argv[ ])
{
    int c;
    while ((c = getopt(argc, argv, "r")) != -1)
    {
        switch (c)
        {
            case 'r':
                REDUCE_FLAG = TRUE;
                break;

            default:
                fprintf(stderr, "Unrecognized flag: %c", c);
                exit(0);
                break;
        }
    }

    extern void yyrestart(FILE *f);
    extern int yyparse();

    // If no files passed as arguments, read from stdin.
    if (optind >= argc)
    {
        yyparse();
    }
    else
    {
        for (; optind < argc; optind++)
        {
            FILE * f = fopen(argv[optind], "r");

            if (!f)
            {
                perror(argv[optind]);
                return 1;
            }

            yyrestart(f);
            yyparse();
            fclose(f);
        }
    }

    return 0;
}

