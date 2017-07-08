/* expr.h - expressions */

extern MEMORY *expr_create(NODE *ast, MEMORY *context);
extern void    expr_to_int(MEMORY *expr, long value);
extern void    expr_to_double(MEMORY *expr, double value);
extern void    expr_to_enum(MEMORY *expr, NODE_ABSTRACTION *abstraction);
extern MEMORY *expr_link(MEMORY *expr);
extern void    expr_unlink(MEMORY *expr);
extern void    expr_merge(MEMORY *expr, MEMORY *context);

extern NODE    expr_int, expr_double;

/* hooks in case you want to experiment with reference counts in GC */
#define expr_link(expr)   (expr)
#define expr_unlink(expr) (void)(expr)
