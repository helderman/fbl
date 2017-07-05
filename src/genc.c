/* genc.c - code generator */

#include <stdio.h>

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "arg.h"
#include "ast.h"

static int genc_any(NODE *ast);

static DEFINE *genc_define(DEFINE *ast, const char *prefix, int chain)
{
	printf("static DEFINE ast%d = INIT_DEFINE(%s%d, AS_NODE(%s%d), \"%s\");\n",
		ast->node_id,
		prefix,
		chain,
		ast->body == NULL ? "" : "&ast",
		ast->body == NULL ? 0 : genc_any(ast->body),
		ast->name);
	return ast;
}

static int genc_defines(DEFINE *ast)
{
	genc_define(ast, "(DEFINE *)", 0);
	while (ast->next != NULL)
	{
		ast = genc_define(ast->next, "&ast", ast->node_id);
	}
	return ast->node_id;
}

static void genc_ident(NODE_IDENT *ast)
{
	printf("static NODE_IDENT ast%d = INIT_IDENT(%s%d, %d, \"%s\");\n",
		ast->node_id,
		ast->parent == NULL ? "(NODE_ABSTRACTION *)" : "&ast",
		ast->parent == NULL ? 0 : ast->parent->node_id,
		ast->index,
		ast->name);
}

static void genc_apply(NODE_APPLY *ast)
{
	printf("static NODE_APPLY ast%d = INIT_NODE(NT_APPLY, %s%d, AS_NODE(&ast%d), AS_NODE(&ast%d));\n",
		ast->node_id,
		ast->parent == NULL ? "(NODE_ABSTRACTION *)" : "&ast",
		ast->parent == NULL ? 0 : ast->parent->node_id,
		genc_any(ast->function),
		genc_any(ast->argument));
}

static void genc_abstraction(const char *nodetype, NODE_ABSTRACTION *ast)
{
	/* forward declaration for the parent chain */
	/* redundant when AST_NAMES is not defined  */
	printf("static NODE_ABSTRACTION ast%d;\n", ast->node_id);

	/* starting a new printf, due to execution order */
	printf("static NODE_ABSTRACTION ast%d = INIT_NODE(%s, %s%d, &ast%d, AS_NODE(&ast%d));\n",
		ast->node_id,
		nodetype,
		ast->parent == NULL ? "(NODE_ABSTRACTION *)" : "&ast",
		ast->parent == NULL ? 0 : ast->parent->node_id,
		genc_defines(ast->define),
		genc_any(ast->body));
}

static void genc_prim(NODE_PRIM *ast)
{
	printf("#define ast%d ast_%s\n", ast->node_id, ast->name);
}

static void genc_int(NODE_INT *ast)
{
	printf("static NODE_INT ast%d = {NT_INT, %ld};\n",
		ast->node_id, ast->value);
}

static void genc_double(NODE_DOUBLE *ast)
{
	printf("static NODE_DOUBLE ast%d = {NT_DOUBLE, %f};\n",
		ast->node_id, ast->value);
}

static int genc_any(NODE *ast)
{
	switch (ast->nodetype)
	{
	case NT_IDENT:
		genc_ident(AS_IDENT(ast));
		break;
	case NT_APPLY:
		genc_apply(AS_APPLY(ast));
		break;
	case NT_LAMBDA:
		genc_abstraction("NT_LAMBDA", AS_ABSTRACTION(ast));
		break;
	case NT_RLET:
		genc_abstraction("NT_RLET", AS_ABSTRACTION(ast));
		break;
	case NT_PRIM:
		genc_prim(AS_PRIM(ast));
		break;
	case NT_INT:
		genc_int(AS_INT(ast));
		break;
	case NT_DOUBLE:
		genc_double(AS_DOUBLE(ast));
		break;
	default:
		/* invalid; ATOM is only used at runtime */
		break;
	}
	return ast->node_id;
}

void genc_tree(NODE *ast)
{
	printf("#include \"fblrt.h\"\n");
	printf("int main(int argc, char *argv[])\n{\n\t"
		"return fblrt_main(AS_NODE(&ast%d), argc, argv);\n}\n",
		genc_any(ast));
	printf("int heap_size = %d;\n", arg_heapsize);
}
