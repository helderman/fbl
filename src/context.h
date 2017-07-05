/* context.h - context */

typedef struct context {
	struct expr *expr;
	struct context *next;
} CONTEXT;

extern CONTEXT *context_create(EXPR *expr, CONTEXT *next);
extern CONTEXT *context_get(CONTEXT *context, int index);
extern CONTEXT *context_relink(CONTEXT *link_me, CONTEXT *unlink_me);

/* hooks in case you want to experiment with reference counts in GC */
#define context_link(context)   (context)
#define context_unlink(context) (void)(context)
