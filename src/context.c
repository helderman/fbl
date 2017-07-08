/* context.c - context */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "heap.h"
#include "context.h"
#include "trace.h"

/* A context is an (indexed) linked list of expressions */
static MEMORY *allocate(MEMORY *expr, MEMORY *next)
{
#ifdef TRACE
	++trace_count_context;
#endif
	return heap_alloc(expr, next);
}

MEMORY *context_create(MEMORY *expr, MEMORY *next)
{
	return allocate(expr, next);
}

MEMORY *context_get(MEMORY *context, int index)
{
	while (index-- > 0)
	{
		assert(context != NULL);
		context = context->m1.mem;
	}
	assert(context != NULL);
	return context;
}

MEMORY *context_relink(MEMORY *link_me, MEMORY *unlink_me)
{
	if (link_me != unlink_me)
	{
		(void)context_link(link_me);
		context_unlink(unlink_me);
	}
	return link_me;
}
