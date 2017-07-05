/* fblrt.c - main module of FBL runtime engine */

#include "ast.h"
#include "heap.h"
#include "reduce.h"

extern NODE *root;

int fblrt_main(NODE *root, int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	heap_init();
	return reduce_tree(root);
}
