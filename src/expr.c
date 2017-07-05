/* expr.c - expressions */

#include <assert.h>
#include <stdlib.h>

#include "ast.h"
#include "enum.h"
#include "expr.h"
#include "context.h"
#include "heap.h"
#include "trace.h"

static EXPR *allocate(NODE *ast, CONTEXT *context)
{
#ifdef TRACE
	++trace_count_expr;
#endif
	return (EXPR *)heap_alloc((MEMORY *)ast, (MEMORY *)context);
}

static EXPR *expr_copy(EXPR *to, EXPR *from)
{
	NODE *ast = to->ast = from->ast;
	if (ast == &expr_int)
	{
		to->dyn.i = from->dyn.i;
	}
	else if (ast == &expr_double)
	{
		to->dyn.d = from->dyn.d;
	}
	else
	{
		assert(ast->nodetype != NT_ATOM);
		to->dyn.context =
			context_relink(from->dyn.context, to->dyn.context);
	}
	return to;
}

EXPR *expr_create(NODE *ast, CONTEXT *context)
{
	return allocate(ast, context);
}

void expr_to_int(EXPR *expr, long value)
{
	assert(expr->ast->nodetype != NT_ATOM);
	context_unlink(expr->dyn.context);
	expr->ast = &expr_int;
	expr->dyn.i = value;
}

void expr_to_double(EXPR *expr, double value)
{
	assert(expr->ast->nodetype != NT_ATOM);
	context_unlink(expr->dyn.context);
	expr->ast = &expr_double;
	expr->dyn.d = value;
}

void expr_to_enum(EXPR *expr, NODE_ABSTRACTION *abstraction)
{
	/* assuming expr is not already a terminal */
	expr->ast = AS_NODE(abstraction);
	context_unlink(expr->dyn.context);
	expr->dyn.context = NULL;
}

/* Keep the ADDRESS of expr, but the CONTENTS of context->expr */
void expr_merge(EXPR *expr, CONTEXT *context)
{
	EXPR *old = context->expr;
	if (old != expr)
	{
		context->expr = expr_link(expr_copy(expr, old));
		expr_unlink(old);
	}
}

/* dummy AST nodes for atomic EXPR nodes */
#ifdef AST_NAMES
#define INIT_ATOM  { NT_ATOM, (NODE_ABSTRACTION *)0 }
#else
#define INIT_ATOM  { NT_ATOM }
#endif
NODE expr_int    = INIT_ATOM;
NODE expr_double = INIT_ATOM;
