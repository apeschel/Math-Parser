#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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

GNode* newast(char op, GNode* l, GNode* r)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = OPERATOR;
    d->oper = op;

    GNode* result = g_node_new(d);
    g_node_prepend(result, l);
    g_node_append(result, r);

    return result;
}

GNode* newnum(t_num n)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = NUMBER;
    d->number = n;

    GNode* result = g_node_new(d);

    return result;
}

GNode* newvar(t_var c)
{
    struct Data* d = malloc(sizeof(struct Data));
    d->type = VARIABLE;
    d->variable = c;

    GNode* result = g_node_new(d);

    return result;
}

static void print_tree_helper(GNode* node, gpointer data)
{
    print_tree(node);
}

void print_tree(GNode* t)
{
    struct Data* d = t->data;
    switch (d->type)
    {
        case NUMBER:
            printf(" %ld", d->number);
            break;

        case VARIABLE:
            printf(" %c", d->variable);
            break;

        case OPERATOR:
            // This is here to prevent a space from appearing at the start
            // of what is output.
            if (!G_NODE_IS_ROOT(t))
            {
                printf(" ");
            }

            printf("(%c", d->oper);
            g_node_children_foreach(t, G_TRAVERSE_ALL, &print_tree_helper, NULL);
            printf(")");
            break;

        default:
            printf("Internal error: unrecognized bad node %c\n", d->type);
            exit(0);
    }
}

static void combine_trees_helper(GNode* child, gpointer parent)
{
    combine_trees(child, (GNode*) parent);
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

static void flatten_tree_helper(GNode* node, gpointer data)
{
    flatten_tree(node);
}

void flatten_tree(GNode* t)
{
    if (G_NODE_IS_LEAF(t))
    {
        return;
    }

    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, &flatten_tree_helper, NULL);
    g_node_children_foreach(t, G_TRAVERSE_NON_LEAVES, &combine_trees_helper, t);
}

/*
struct ast* reduce(struct ast* t)
{
    // Only reduce if this is an AST node.
    switch(t->node_type)
    {
        case NUM_NODE:
        case VAR_NODE:
            return t;

        case AST:
            break;

        default:
            printf("Internal error: unrecognized bad node %c\n", t->node_type);
            exit(0);
    }

    // Reduce children
    t->left = reduce(t->left);
    t->right = reduce(t->right);

    t = numeric_reduce(t);
    t = commutative_reduce(t);
    return t;
}

// This function simplifies pure numeric subexpressions.
// Parameter t must be an AST node.
struct ast* numeric_reduce(struct ast* t)
{
    struct num_node *left_node = (struct num_node *) t->left;
    struct num_node *right_node = (struct  num_node *) t->right;

    // Only reduce if both children are numeric.
    if (left_node->node_type  != NUM_NODE ||
        right_node->node_type != NUM_NODE)
    {
        return t;
    }

    t_num left_val = left_node->number;
    t_num right_val = right_node->number;
    t_num result;

    switch (t->oper)
    {
        case '+':
            result = left_val + right_val;
            break;

        case '-':
            result = left_val - right_val;
            break;

        case '*':
            result = left_val * right_val;
            break;

        case '/':
            result = floor(left_val / right_val);
            break;

        case '^':
            result = floor(pow(left_val, right_val));
            break;

        default:
            printf("Internal error: unrecognized operator :%c\n", t->oper);
            exit(0);
    }

    free_tree(t);
    return (struct ast *)newnum(result);
}
*/

