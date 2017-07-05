/* reduce.c - interpreter and runtime engine for compiled code */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "expr.h"
#include "context.h"
#include "stack.h"
#include "trace.h"

static void reduce_ident(EXPR *expr)
{
	expr_merge(expr, context_get(expr->dyn.context, AS_IDENT(expr->ast)->index));
}

static void reduce_apply(EXPR *expr)
{
	/* The order is essential: create AND attach new stack node BEFORE
	** creating expression node. Otherwise GC may wipe the latter.
	*/
	EXPR **tos = stack_push();
	*tos = expr_create(
		AS_APPLY(expr->ast)->function,
		context_link(expr->dyn.context));
}

static void reduce_lambda(EXPR *expr)		/* beta reduction */
{
	EXPR *arg;
	EXPR *new_tos = stack_peek();
	assert(new_tos->ast->nodetype == NT_APPLY);
	arg = expr_create(
		AS_APPLY(new_tos->ast)->argument,
		new_tos->dyn.context);
	new_tos->ast = AS_ABSTRACTION(expr->ast)->body;
	new_tos->dyn.context = expr->dyn.context;
	stack_pop_push(arg);	/* prevent arg being wiped by GC */
	new_tos->dyn.context = context_create(arg, new_tos->dyn.context);
	stack_pop();
}

static void reduce_rlet(EXPR *expr)
{
	CONTEXT *context;
	NODE_ABSTRACTION *ast = AS_ABSTRACTION(expr->ast);
	DEFINE *def;
	for (def = ast->define; def != NULL; def = def->next)
	{
		expr->dyn.context = context_create(NULL, expr->dyn.context);
		expr->dyn.context->expr = expr_create(def->body, NULL);
	}
	expr->ast = ast->body;
	context = expr->dyn.context;
	for (def = ast->define; def != NULL; def = def->next)
	{
		context->expr->dyn.context = context_link(expr->dyn.context);
		context = context->next;
	}
	context_unlink(context);
	context_unlink(expr->dyn.context);
}

static void reduce_expr(EXPR *expr)
{
	assert(expr != NULL);
	assert(expr->ast != NULL);
	switch (expr->ast->nodetype)
	{
	case NT_PRIM:
		stack_pop();
		AS_PRIM(expr->ast)->func();
		expr_unlink(expr);
		break;
	case NT_IDENT:
		reduce_ident(expr);
		break;
	case NT_INT:
		expr_to_int(expr, AS_INT(expr->ast)->value);
		break;
	case NT_DOUBLE:
		expr_to_double(expr, AS_DOUBLE(expr->ast)->value);
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
		fprintf(stderr, "Unknown node type %d\n", expr->ast->nodetype);
		exit(1);
	}
}

int reduce_tree(NODE *ast)
{
	EXPR **tos = stack_push();
	EXPR *expr = *tos = expr_create(ast, NULL);
	while (stack_not_empty())
	{
#ifdef TRACE
		trace_output_counts();
#endif
		reduce_expr(stack_tos());
	}
	assert(expr->ast == &expr_int);
#ifdef TRACE
	trace_output_counts();
	trace_exit(expr->dyn.i);
#endif
	return (int)expr->dyn.i;
}
