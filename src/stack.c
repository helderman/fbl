/* stack.c - execution stack */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "heap.h"
#include "expr.h"
#include "stack.h"
#include "trace.h"

static MEMORY *stack = NULL;

/* A stack is a linked list of expressions */
static MEMORY *allocate(MEMORY *next)
{
	return heap_alloc(NULL, next);
}

int stack_not_empty(void)
{
	return stack != NULL;
}

MEMORY *stack_tos(void)
{
	return stack->m0.mem;
}

MEMORY *stack_peek(void)
{
	assert(stack != NULL && stack->m1.mem != NULL);
	return stack->m1.mem->m0.mem;
}

MEMORY **stack_push(void)
{
	stack = allocate(stack);
#ifdef TRACE
	if (trace_count_max_stack < ++trace_count_stack) 
	{
		trace_count_max_stack = trace_count_stack;
	}
#endif
	return &stack->m0.mem;
}

MEMORY *stack_pop(void)
{
	MEMORY *old_tos = stack;
	MEMORY *expr = stack->m0.mem;
	stack = stack->m1.mem;
	stack_unlink(old_tos);
	heap_free(old_tos);
#ifdef TRACE
	--trace_count_stack;
#endif
	return expr;
}

void stack_pop_push(MEMORY *expr)
{
	stack->m0.mem = expr;
}

MEMORY *stack_pointer(void)
{
	return stack;
}
