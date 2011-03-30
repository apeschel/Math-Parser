#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "funcs.h"

void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

struct ast* newast(char op, struct ast* l, struct ast* r)
{
    struct ast *result = malloc(sizeof(struct ast));

    if (!result)
    {
        yyerror("out of space");
        exit(0);
    }

    result->node_type = AST;
    result->oper = op;
    result->left = l;
    result->right = r;

    return result;
}

struct ast* newnum(long d)
{
    struct num_node *result = malloc(sizeof(struct num_node));

    if (!result)
    {
        yyerror("out of space");
        exit(0);
    }

    result->node_type = NUM_NODE;
    result->number = d;

    return (struct ast *)result;
}

struct ast* newvar(char c)
{
    struct var_node *result = malloc(sizeof(struct var_node));

    if (!result)
    {
        yyerror("out of space");
        exit(0);
    }

    result->node_type = VAR_NODE;
    result->variable = c;

    return (struct ast *)result;
}

void print_tree(struct ast* t)
{
    switch (t->node_type)
    {
        case NUM_NODE:
            printf("%ld", ((struct num_node *)t)->number);
            break;

        case VAR_NODE:
            printf("%c", ((struct var_node *)t)->variable);
            break;

        case AST:
            printf("(%c ", t->oper);
            print_tree(t->left);
            printf(" ");
            print_tree(t->right);
            printf(")");
            break;

        default:
            printf("Internal error: unrecognized bad node %c\n", t->node_type);
            exit(0);
    }
}

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

    long left_val = left_node->number;
    long right_val = right_node->number;
    long result;

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

void free_tree(struct ast* t)
{
    switch(t->node_type)
    {
        case AST:
            free_tree(t->left);
            free_tree(t->right);
        case NUM_NODE:
        case VAR_NODE:
            free(t);
            break;
        default:
            printf("Internal error: free bad node %c\n", t->node_type);
    }
}
