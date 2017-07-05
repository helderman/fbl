/* fblc.c - main module of FBL compiler
**
** Reads, tokenizes and parses one or more FBL source files,
** builds up a parse tree, and generates C code.
*/

#include <stdio.h>
#include <string.h>

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "arg.h"
#include "ast.h"
#include "parse.h"
#include "alpha.h"
#include "typechk.h"
#include "genc.h"

static void compile(void)
{
	NODE *ast = parse_root();
	alpha_tree(ast);
	check_types(ast);
	genc_tree(ast);
}

int main(int argc, char *argv[])
{
	(void)argc;
	arg_init(argv);
	compile();
	return 0;
}
