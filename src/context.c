/* context.c - context */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "expr.h"
#include "context.h"
#include "heap.h"
#include "trace.h"

static CONTEXT *allocate(EXPR *expr, CONTEXT *next)
{
#ifdef TRACE
	++trace_count_context;
#endif
	return (CONTEXT *)heap_alloc((MEMORY *)expr, (MEMORY *)next);
}

CONTEXT *context_create(EXPR *expr, CONTEXT *next)
{
	return allocate(expr, next);
}

CONTEXT *context_get(CONTEXT *context, int index)
{
	while (index-- > 0)
	{
		assert(context != NULL);
		context = context->next;
	}
	assert(context != NULL);
	return context;
}

CONTEXT *context_relink(CONTEXT *link_me, CONTEXT *unlink_me)
{
	if (link_me != unlink_me)
	{
		(void)context_link(link_me);
		context_unlink(unlink_me);
	}
	return link_me;
}
