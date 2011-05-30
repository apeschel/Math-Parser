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
