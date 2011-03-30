extern int yylineno;
void yyerror(char *s, ...);

typedef long t_num;
typedef char t_var;

enum ast_types
{
    AST,
    NUM_NODE,
    VAR_NODE
};

struct ast
{
    enum ast_types  node_type;
    char            oper;
    struct ast*     left;
    struct ast*     right;
};

struct num_node
{
    enum ast_types  node_type;
    t_num           number;
};

struct var_node
{
    enum ast_types  node_type;
    t_var           variable;
};

struct ast* newast(char op, struct ast* l, struct ast* r);
struct ast* newnum(t_num d);
struct ast* newvar(t_var c);

void print_tree(struct ast* t);
void free_tree(struct ast* t);

struct ast* reduce(struct ast* t);
struct ast* numeric_reduce(struct ast* t);
struct ast* commutative_reduce(struct ast* t);
