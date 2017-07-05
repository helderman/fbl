/* stack.c - execution stack */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "expr.h"
#include "stack.h"
#include "heap.h"
#include "trace.h"

static STACK *stack = NULL;

static STACK *allocate(STACK *next)
{
	return (STACK *)heap_alloc(NULL, (MEMORY *)next);
}

int stack_not_empty(void)
{
	return stack != NULL;
}

EXPR *stack_tos(void)
{
	return stack->expr;
}

EXPR *stack_peek(void)
{
	assert(stack != NULL && stack->next != NULL);
	return stack->next->expr;
}

EXPR **stack_push(void)
{
	stack = allocate(stack);
#ifdef TRACE
	if (trace_count_max_stack < ++trace_count_stack) 
	{
		trace_count_max_stack = trace_count_stack;
	}
#endif
	return &stack->expr;
}

EXPR *stack_pop(void)
{
	STACK *old_tos = stack;
	EXPR *expr = stack->expr;
	stack = stack->next;
	stack_unlink(old_tos);
	heap_free((MEMORY *)old_tos);
#ifdef TRACE
	--trace_count_stack;
#endif
	return expr;
}

void stack_pop_push(EXPR *expr)
{
	stack->expr = expr;
}

STACK *stack_pointer(void)
{
	return stack;
}
