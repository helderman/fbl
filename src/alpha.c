/* alpha.c - alpha conversion; uniquely identify variables
**
** Two or more different variables can have the same name,
** both in disjunct scopes and in nested scopes.
**
** Here we traverse the entire AST and assign to every variable
** a unique numeric ID (starting with 1).
** Of course, nodes that refer to the same variable get the same ID.
**
** To be able to match a variable referenced in an inner scope with its
** definition in an outer scope, we also build up chains of nested scopes;
** we populate the 'parent' field of every NT_IDENT/APPLY/LAMBDA/RLET node,
** pointing to the closest surrounding NT_LAMBDA or NT_RLET node.
**
** Strictly speaking, the 'parent' fields in NT_IDENT and NT_APPLY nodes are
** only necessary for debugging purposes. We do use them in the code
** generator as well (genc.c), but that was not essential; we could have 
** accomplished the same with an extra parameter 'parent' in those functions.
*/

#include <stdio.h>
#include <string.h>

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "ast.h"

static void alpha_ident(NODE_IDENT *ast, NODE_ABSTRACTION *parent)
{
	int index = 0;
	for (; parent != NULL; parent = parent->parent)
	{
		DEFINE *define = parent->define;
		for (; define != NULL; define = define->next, index++)
		{
			if (strcmp(ast->name, define->name) == 0)
			{
				ast->defined_by = define;
				ast->index = index;
				return;
			}
		}
	}
	fprintf(stderr, "Undefined identifier: '%s'\n", ast->name);
}

static void alpha_any(NODE *ast, NODE_ABSTRACTION *parent, int *node_id);

static void alpha_abstraction(NODE_ABSTRACTION *ast, int *node_id)
{
	DEFINE *def;
	alpha_any(ast->body, ast, node_id);
	for (def = ast->define; def != NULL; def = def->next)
	{
		def->node_id = ++*node_id;
		if (def->body != NULL)
		{
			alpha_any(def->body, ast, node_id);
		}
	}
}

static void alpha_any(NODE *ast, NODE_ABSTRACTION *parent, int *node_id)
{
	ast->node_id = ++*node_id;
	if (USES_CONTEXT(ast->nodetype))
	{
		ast->parent = parent;
	}
	switch (ast->nodetype)
	{
	case NT_IDENT:
		alpha_ident(AS_IDENT(ast), parent);
		break;
	case NT_APPLY:
		alpha_any(AS_APPLY(ast)->function, parent, node_id);
		alpha_any(AS_APPLY(ast)->argument, parent, node_id);
		break;
	case NT_LAMBDA:
	case NT_RLET:
		alpha_abstraction(AS_ABSTRACTION(ast), node_id);
		break;
	default:
		break;      /* NT_INT, NT_DOUBLE: nothing to do */
	}
}

void alpha_tree(NODE *ast)
{
	int node_id = 0;
	alpha_any(ast, NULL, &node_id);
}
