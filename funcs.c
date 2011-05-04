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
    d->var_name = c;
    d->sign = POSITIVE;

    return d;
}

GNode* newvar(t_var c)
{
    struct Data* d = new_var_data(c);
    GNode* result = g_node_new(d);

    return result;
}

void print_tree(const GNode* t)
{
    struct Data* d = t->data;
    switch (d->type)
    {
        case NUMBER:
            printf(" %f", d->number);
            break;

        case VARIABLE:
            printf(" %s%c", d->sign == NEGATIVE? "-" : "", d->var_name);
            break;

        case OPERATOR:
            // This is here to prevent a space from appearing at the start
            // of what is output.
            if (!G_NODE_IS_ROOT(t))
            {
                printf(" ");
            }

            printf("(%c", d->oper);
            g_node_children_foreach((GNode*) t, G_TRAVERSE_ALL, (GNodeForeachFunc) &print_tree, NULL);
            printf(")");
            break;

        default:
            printf("Internal error: unrecognized bad node %c\n", d->type);
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

    while (g_node_n_children(child) > 0)
    {
        GNode* grandchild = g_node_first_child(child);
        g_node_unlink(grandchild);
        g_node_insert_before(parent, child, grandchild);
    }

    g_node_destroy(child);
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
        void (*num_operator)(GNode* t),
        void (*var_operator)(GNode* t)
    )
{
    struct Data* d = t->data;
    GNode* left_child = g_node_nth_child(t, LEFT);
    GNode* right_child = g_node_nth_child(t, RIGHT);

    switch(d->type)
    {
        case NUMBER:
            num_operator(t);
            return;

        case VARIABLE:
            var_operator(t);
            return;

        case OPERATOR:
            inversion_func(left_child, num_operator, var_operator);
            inversion_func(right_child, num_operator, var_operator);
            return;
    }
}

static void invert_num(GNode* t)
{
    struct Data* d = t->data;
    d->number = 1 / d->number;
}

static void invert_var(GNode* t)
{
    // struct Data* d = t->data;
    // TODO: fill this in.
}

void invert(GNode* t)
{
    inversion_func(t, invert_num, invert_var);
}

static void negate_num(GNode *t)
{
    struct Data* d = t->data;
    d->number = -(d->number);
}

static void negate_var(GNode *t)
{
    struct Data* d = t->data;

    switch (d->sign)
    {
        case POSITIVE:
            d->sign = NEGATIVE;
            return;

        case NEGATIVE:
            d->sign = POSITIVE;
            return;

        default:
            return;
    }
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

static void reduce_func(GNode* t, t_num sum, t_num (*operator)(t_num x, t_num y))
{
    GNode* child = t->children;
    GNode* next_child;

    while (NULL != child)
    {
        struct Data* d = child->data;
        switch (d->type)
        {
            case NUMBER:
                sum = operator(sum, d->number);
                next_child = child->next;
                g_node_destroy(child);
                break;

            case VARIABLE:
                next_child = child->next;
                break;

            case OPERATOR:
                next_child = child->next;
                break;
        }

        child = next_child;
    }

    if (g_node_n_children(t) == 0)
    {
        free(t->data);
        t->data = new_num_data(sum);
    } else
    {
        g_node_append(t, newnum(sum));
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
            reduce_func(t, 0, &add_op);
            break;

        case '*':
            reduce_func(t, 1, &mul_op);
            return;

        case '^':
            // TODO Fill this in.
            return;

        default:
            return;
    }
}
