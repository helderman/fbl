/* reduce.c - interpreter and runtime engine for compiled code */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "heap.h"
#include "expr.h"
#include "context.h"
#include "stack.h"
#include "trace.h"

static void reduce_ident(MEMORY *expr)
{
	expr_merge(
		expr,
		context_get(expr->m1.mem, AS_IDENT(expr->m0.ast)->index));
}

static void reduce_apply(MEMORY *expr)
{
	/* The order is essential: create AND attach new stack node BEFORE
	** creating expression node. Otherwise GC may wipe the latter.
	*/
	MEMORY **tos = stack_push();
	*tos = expr_create(
		AS_APPLY(expr->m0.ast)->function,
		context_link(expr->m1.mem));
}

static void reduce_lambda(MEMORY *expr)		/* beta reduction */
{
	MEMORY *arg;
	MEMORY *new_tos = stack_peek();
	assert(new_tos->m0.ast->nodetype == NT_APPLY);
	arg = expr_create(
		AS_APPLY(new_tos->m0.ast)->argument,
		new_tos->m1.mem);
	new_tos->m0.ast = AS_ABSTRACTION(expr->m0.ast)->body;
	new_tos->m1.mem = expr->m1.mem;
	stack_pop_push(arg);	/* prevent arg being wiped by GC */
	new_tos->m1.mem = context_create(arg, new_tos->m1.mem);
	stack_pop();
}

static void reduce_rlet(MEMORY *expr)
{
	MEMORY *context;
	NODE_ABSTRACTION *ast = AS_ABSTRACTION(expr->m0.ast);
	DEFINE *def;
	for (def = ast->define; def != NULL; def = def->next)
	{
		expr->m1.mem = context_create(NULL, expr->m1.mem);
		expr->m1.mem->m0.mem = expr_create(def->body, NULL);
	}
	expr->m0.ast = ast->body;
	context = expr->m1.mem;
	for (def = ast->define; def != NULL; def = def->next)
	{
		context->m0.mem->m1.mem = context_link(expr->m1.mem);
		context = context->m1.mem;	/* next */
	}
	context_unlink(context);
	context_unlink(expr->m1.mem);
}

static void reduce_expr(MEMORY *expr)
{
	assert(expr != NULL);
	assert(expr->m0.ast != NULL);
	switch (expr->m0.ast->nodetype)
	{
	case NT_PRIM:
		stack_pop();
		AS_PRIM(expr->m0.ast)->func();
		expr_unlink(expr);
		break;
	case NT_IDENT:
		reduce_ident(expr);
		break;
	case NT_INT:
		expr_to_int(expr, AS_INT(expr->m0.ast)->value);
		break;
	case NT_DOUBLE:
		expr_to_double(expr, AS_DOUBLE(expr->m0.ast)->value);
		break;
	case NT_APPLY:
		reduce_apply(expr);
		break;
	case NT_LAMBDA:
		reduce_lambda(expr);
		break;
	case NT_RLET:
		reduce_rlet(expr);
		break;
	case NT_ATOM:
		stack_pop();	/* atom = result of strict evaluation */
		break;
	default:
		fprintf(stderr, "Unknown node type %d\n", expr->m0.ast->nodetype);
		exit(1);
	}
}

int reduce_tree(NODE *ast)
{
	MEMORY **tos = stack_push();
	MEMORY *expr = *tos = expr_create(ast, NULL);
	while (stack_not_empty())
	{
#ifdef TRACE
		trace_output_counts();
#endif
		reduce_expr(stack_tos());
	}
	assert(expr->m0.ast == &expr_int);
#ifdef TRACE
	trace_output_counts();
	trace_exit(expr->m1.i);
#endif
	return (int)expr->m1.i;
}
