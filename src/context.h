/* context.h - context */

extern MEMORY *context_create(MEMORY *expr, MEMORY *next);
extern MEMORY *context_get(MEMORY *context, int index);
extern MEMORY *context_relink(MEMORY *link_me, MEMORY *unlink_me);

/* hooks in case you want to experiment with reference counts in GC */
#define context_link(context)   (context)
#define context_unlink(context) (void)(context)
