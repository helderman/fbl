/* trace.c - trace evaluation process */

#ifdef TRACE

#include <stdio.h>

#include "ast.h"
#include "heap.h"
#include "expr.h"
#include "context.h"
#include "stack.h"

int trace_count_expr = 0;
int trace_count_context = 0;
int trace_count_stack = 0;
int trace_count_max_stack = 0;
int trace_count_gc = 0;

static void print_ast(FILE *fp, NODE *ast)
{
	DEFINE *def;
	switch (ast->nodetype)
	{
	case NT_IDENT:
#ifdef AST_NAMES
		fprintf(fp, "%s", AS_IDENT(ast)->name);
#else
		fprintf(fp, "var[%d]", AS_IDENT(ast)->index);
#endif
		break;
	case NT_APPLY:
		fprintf(fp, "(");
		print_ast(fp, AS_APPLY(ast)->function);
		fprintf(fp, " ");
		print_ast(fp, AS_APPLY(ast)->argument);
		fprintf(fp, ")");
		break;
	case NT_LAMBDA:
		fprintf(fp, "\\");
#ifdef AST_NAMES
		fprintf(fp, "%s ", AS_ABSTRACTION(ast)->define->name);
#else
		fprintf(fp, "var ");
#endif
		print_ast(fp, AS_ABSTRACTION(ast)->body);
		break;
	case NT_RLET:
		fprintf(fp, "(");
		print_ast(fp, AS_ABSTRACTION(ast)->body);
		for (def = AS_ABSTRACTION(ast)->define; def != NULL; def = def->next)
		{
#ifdef AST_NAMES
			fprintf(fp, " : %s ", def->name);
#else
			fprintf(fp, " : var ");
#endif
			print_ast(fp, def->body);
		}
		fprintf(fp, ")");
		break;
	case NT_PRIM:
#ifdef AST_NAMES
		fprintf(fp, "$%s", AS_PRIM(ast)->name);
#else
		fprintf(fp, "$prim");
#endif
		break;
	case NT_INT:
		fprintf(fp, "%ld", AS_INT(ast)->value);
		break;
	case NT_DOUBLE:
		fprintf(fp, "%f", AS_DOUBLE(ast)->value);
		break;
	case NT_ATOM:
		fprintf(fp, "atom");
		break;
	}
}

static void print_context(FILE *fp, NODE_ABSTRACTION *ast, MEMORY *context)
{
	int index = 0;
	fprintf(fp, " [");
	for (; ast != NULL; ast = ast->parent)
	{
		DEFINE *define = ast->define;
		for (; define != NULL; define = define->next, index++)
		{
			if (index > 0) fprintf(fp, ", ");
#ifdef AST_NAMES
			fprintf(fp, "%s", define->name);
#else
			fprintf(fp, "@%d", index);
#endif
			if (context == NULL)
			{
				fprintf(fp, "=???");
			}
			else
			{
				if (context->m0.mem->m0.ast == &expr_int)
				{
					fprintf(fp, "=%ld", context->m0.mem->m1.i);
				}
				else if (context->m0.mem->m0.ast == &expr_double)
				{
					fprintf(fp, "=%f", context->m0.mem->m1.d);
				}
				context = context->m1.mem;	/* next */
			}
		}
	}
	if (context != NULL)
	{
		if (index > 0) fprintf(fp, ", ");
		fprintf(fp, "...");
	}
	fprintf(fp, "]");
}

void trace_output_counts(void)
{
	static int step = 0;
	MEMORY *stack;
	fprintf(stderr, "#%d: expr %d, context %d, stack %d, GCs %d\n",
		++step, trace_count_expr, trace_count_context, trace_count_stack, trace_count_gc);
	for (stack = stack_pointer(); stack != NULL; stack = stack->m1.mem)
	{
		MEMORY *expr = stack->m0.mem;
		fprintf(stderr, "- ");
		print_ast(stderr, expr->m0.ast);
		if (expr->m0.ast == &expr_int)
		{
			fprintf(stderr, " %ld", expr->m1.i);
		}
		else if (expr->m0.ast == &expr_double)
		{
			fprintf(stderr, " %f", expr->m1.d);
		}
		else if (USES_CONTEXT(expr->m0.ast->nodetype))
		{
			print_context(stderr, expr->m0.ast->parent, expr->m1.mem);
		}
		fprintf(stderr, "\n");
	}
}

void trace_exit(long exitcode)
{
	fprintf(stderr, "Max stack depth = %d, exit code = %ld\n",
		trace_count_max_stack, exitcode);
}

#endif
