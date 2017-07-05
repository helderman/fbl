/* ast.c - abstract syntax tree, a.k.a. parse tree
**
** Collection of functions to create objects representing AST nodes.
** This is used to build up the parse tree while parsing FBL source files.
*/

#include <stdlib.h>
#include <string.h>

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "ast.h"

DEFINE *create_define(DEFINE *next, NODE *body, const char *name)
{
	DEFINE *define = malloc(sizeof(DEFINE));
	define->next = next;
	define->body = body;
	define->name = name;
	return define;
}

NODE *create_prim(const char *name)
{
	NODE_PRIM *ast = malloc(sizeof(NODE_PRIM));
	ast->nodetype = NT_PRIM;
	ast->name = name;
	return AS_NODE(ast);
}

NODE *create_ident(const char *name)
{
	NODE_IDENT *ast = malloc(sizeof(NODE_IDENT));
	ast->nodetype = NT_IDENT;
	ast->parent = NULL;
	ast->index = 0;
	ast->node_id = 0;
	ast->name = name;
	return AS_NODE(ast);
}

NODE *create_int(long value)
{
	NODE_INT *ast = malloc(sizeof(NODE_INT));
	ast->nodetype = NT_INT;
	ast->value = value;
	return AS_NODE(ast);
}

NODE *create_double(double value)
{
	NODE_DOUBLE *ast = malloc(sizeof(NODE_DOUBLE));
	ast->nodetype = NT_DOUBLE;
	ast->value = value;
	return AS_NODE(ast);
}

NODE *create_string(const char *s)
{
	NODE *list = create_ident("Nil");
	int i = strlen(s);
	while (--i >= 0)
	{
		list = create_apply(
			create_apply(create_ident("Cons"), create_int((long)s[i])),
			list);
	}
	return list;
}

NODE *create_apply(NODE *function, NODE *argument)
{
	NODE_APPLY *ast = malloc(sizeof(NODE_APPLY));
	ast->nodetype = NT_APPLY;
	ast->parent = NULL;
	ast->function = function;
	ast->argument = argument;
	return AS_NODE(ast);
}

NODE *create_lambda(NODE *body, const char *name)
{
	NODE_ABSTRACTION *ast = malloc(sizeof(NODE_ABSTRACTION));
	ast->nodetype = NT_LAMBDA;
	ast->define = create_define(NULL, NULL, name);
	ast->parent = NULL;
	ast->body = body;
	return AS_NODE(ast);
}

NODE *create_rlet(NODE *body, DEFINE *define)
{
	if (define == NULL)
	{
		return body;
	}
	else
	{
		NODE_ABSTRACTION *ast = malloc(sizeof(NODE_ABSTRACTION));
		ast->nodetype = NT_RLET;
		ast->define = define;
		ast->parent = NULL;
		ast->body = body;
		return AS_NODE(ast);
	}
}
