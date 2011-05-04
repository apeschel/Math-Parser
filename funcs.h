#ifndef FUNCS_H
#define FUNCS_H
#include <glib.h>

extern int yylineno;
void yyerror(char *s, ...);

typedef long t_num;
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
            t_var var_name;
            enum
            {
                POSITIVE,
                NEGATIVE
            } sign;
        };
    };
};

GNode* newast(char op, GNode* l, GNode* r);
GNode* newnum(t_num d);
GNode* newvar(t_var c);

void print_tree(GNode* t);
void combine_trees(GNode* child, GNode* parent);
void flatten_tree(GNode* t);

void simplify_op(GNode* t);
void invert(GNode* t);
void negate(GNode* t);

GNode* reduce(GNode* t);
GNode* numeric_reduce(GNode* t);

#endif // FUNCS_H
