/* typechk.c - type checker */

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "ast.h"

void check_types(NODE *ast)
{
	//DATATYPE type;
	switch (ast->nodetype)
	{
	case NT_PRIM:
		// TODO
		break;
	case NT_IDENT:
		// TODO
		break;
	case NT_INT:
		// TODO
		break;
	case NT_DOUBLE:
		// TODO
		break;
	case NT_APPLY:
		// TODO
		break;
	case NT_LAMBDA:
		// TODO
		break;
	case NT_RLET:
		// TODO
		break;
	default:
		// illegal; NT_ATOM is only used at runtime
		break;
	}
}
