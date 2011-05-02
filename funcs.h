#ifndef FUNCS_H
#define FUNCS_H
#include <glib.h>

extern int yylineno;
void yyerror(char *s, ...);

typedef long t_num;
typedef char t_var;

enum NodeType
{
    OPERATOR,
    NUMBER,
    VARIABLE
};

struct Data
{
    enum NodeType type;
    union
    {
        char oper;
        t_num number;
        t_var variable;
    };
};

GNode* newast(char op, GNode* l, GNode* r);
GNode* newnum(t_num d);
GNode* newvar(t_var c);

void print_tree(GNode* t);

GNode* reduce(GNode* t);
GNode* numeric_reduce(GNode* t);

#endif // FUNCS_H
