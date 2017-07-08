/* expr.c - expressions */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "heap.h"
#include "enum.h"
#include "expr.h"
#include "context.h"
#include "trace.h"

/* An expresion is a tuple (AST, context) */
static MEMORY *allocate(NODE *ast, MEMORY *context)
{
#ifdef TRACE
	++trace_count_expr;
#endif
	return heap_alloc((MEMORY *)ast, context);
}

static MEMORY *expr_copy(MEMORY *to, MEMORY *from)
{
	NODE *ast = to->m0.ast = from->m0.ast;
	if (ast == &expr_int)
	{
		to->m1.i = from->m1.i;
	}
	else if (ast == &expr_double)
	{
		to->m1.d = from->m1.d;
	}
	else
	{
		assert(ast->nodetype != NT_ATOM);
		to->m1.mem = context_relink(from->m1.mem, to->m1.mem);
	}
	return to;
}

MEMORY *expr_create(NODE *ast, MEMORY *context)
{
	return allocate(ast, context);
}

void expr_to_int(MEMORY *expr, long value)
{
	assert(expr->m0.ast->nodetype != NT_ATOM);
	context_unlink(expr->m1.mem);
	expr->m0.ast = &expr_int;
	expr->m1.i = value;
}

void expr_to_double(MEMORY *expr, double value)
{
	assert(expr->m0.ast->nodetype != NT_ATOM);
	context_unlink(expr->m1.mem);
	expr->m0.ast = &expr_double;
	expr->m1.d = value;
}

void expr_to_enum(MEMORY *expr, NODE_ABSTRACTION *abstraction)
{
	/* assuming expr is not already a terminal */
	expr->m0.ast = AS_NODE(abstraction);
	context_unlink(expr->m1.mem);
	expr->m1.mem = NULL;
}

/* Keep the ADDRESS of expr, but the CONTENTS of context->m0 */
void expr_merge(MEMORY *expr, MEMORY *context)
{
	MEMORY *old_expr = context->m0.mem;
	if (old_expr != expr)
	{
		context->m0.mem = expr_link(expr_copy(expr, old_expr));
		expr_unlink(old_expr);
	}
}

/* dummy AST nodes for atomic expression nodes */
#ifdef AST_NAMES
#define INIT_ATOM  { NT_ATOM, (NODE_ABSTRACTION *)0 }
#else
#define INIT_ATOM  { NT_ATOM }
#endif
NODE expr_int    = INIT_ATOM;
NODE expr_double = INIT_ATOM;
