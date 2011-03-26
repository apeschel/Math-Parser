#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
            printf(" %d ", ((struct num_node *)t)->number);
            break;

        case VAR_NODE:
            printf(" %c ", ((struct var_node *)t)->variable);
            break;

        case AST:
            printf("( %c ", t->oper);
            print_tree(t->left);
            print_tree(t->right);
            printf(")");
            break;

        default:
            printf("Internal error: free bad node %c\n", t->node_type);
            break;
    }
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
