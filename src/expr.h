/* expr.h - expressions */

typedef struct expr {
	NODE *ast;
	union {
		struct context *context;
		long i;
		double d;
	} dyn;
} EXPR;

extern EXPR *expr_create(NODE *ast, struct context *context);
extern void  expr_to_int(EXPR *expr, long value);
extern void  expr_to_double(EXPR *expr, double value);
extern void  expr_to_enum(EXPR *expr, NODE_ABSTRACTION *abstraction);
extern EXPR *expr_link(EXPR *expr);
extern void  expr_unlink(EXPR *expr);
extern void  expr_merge(EXPR *expr, struct context *context);

extern NODE  expr_int, expr_double;

/* hooks in case you want to experiment with reference counts in GC */
#define expr_link(expr)   (expr)
#define expr_unlink(expr) (void)(expr)
