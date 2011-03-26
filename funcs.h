extern int yylineno;
void yyerror(char *s, ...);

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
    int             number;
};

struct var_node
{
    enum ast_types  node_type;
    char            variable;
};

struct ast* newast(char op, struct ast* l, struct ast* r);
struct ast* newnum(long d);
struct ast* newvar(char c);

void print_tree(struct ast* t);
void free_tree(struct ast* t);
