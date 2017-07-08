/* strict.c - strict evaluation
**
** FBL is lazy by nature; an expression is NOT automatically evaluated
** when passed as a function parameter, unless we explicitly say so.
** For this purpose we have a primitive function $strict (exposed by prim.c).
**
** Usage: ($strict A F) - evaluates A, then applies (F A).
** Note: A must be of an atomic type, i.e. integer or double.
**
** So why not ($strict F A)? The order of the parameters is intentional;
** it allows for the simplest implementation of $strict.
**
** Stack before reduction of $strict (similar to reduction of NT_LAMBDA):
** - $strict
** - ($strict A)
** - (($strict A) F)
**
** Stack after reduction of $strict:
** - A
** - \_func (_func _arg)         with context [_arg = A]
** - (($strict A) F)
**
** So what happens next?
** - Since A is on top, it is evaluated first.
**   A is supposed to have an atomic type (integer, double),
**   so its evaluation should end with an atomic expression (node NT_ATOM).
** - The reducer pops the atom from the stack.
** - The reducer reduces \_func (_func _arg) together with (($strict A) F)
**   to (_func _arg) with context [_func = F, _arg = A],
**   which effectively equals (F A).
*/

#include <assert.h>

#include "ast.h"
#include "heap.h"
#include "expr.h"
#include "context.h"

/* The lambda abstraction \_func (_func _arg) is static;
** it can be used every time we are reducing a strict argument.
*/
#ifdef AST_NAMES
static DEFINE def_arg = INIT_DEFINE((DEFINE *)0, AS_NODE(0), "_arg");
static NODE_ABSTRACTION ast_parent = INIT_NODE(NT_LAMBDA, (NODE_ABSTRACTION *)0, &def_arg, AS_NODE(0));
static NODE_ABSTRACTION ast_lambda;	/* forward declaration */
#endif

static DEFINE     def_func  = INIT_DEFINE((DEFINE *)0, AS_NODE(0), "_func");
static NODE_IDENT ast_func  = INIT_IDENT(&ast_lambda, 0, "_func");
static NODE_IDENT ast_arg   = INIT_IDENT(&ast_lambda, 1, "_arg");
static NODE_APPLY ast_apply = INIT_NODE(NT_APPLY, &ast_lambda, AS_NODE(&ast_func), AS_NODE(&ast_arg));
static NODE_ABSTRACTION ast_lambda = INIT_NODE(NT_LAMBDA, &ast_parent, &def_func, AS_NODE(&ast_apply));

static NODE *get_arg(MEMORY *expr)
{
	assert(expr->m0.ast->nodetype == NT_APPLY);
	return AS_APPLY(expr->m0.ast)->argument;
}

/* Caller has already popped $strict, has pushed a new blank stack element,
** and has passed a pointer to ($strict A) through parameter expr.
** We overwrite this expr with: \_func (_func _arg), with context [_arg = A].
** We populate the blank stack element with A.
*/
void strict_reduction(MEMORY **tos, MEMORY *expr)
{
	*tos = expr_create(get_arg(expr), expr->m1.mem);
	expr->m0.ast = AS_NODE(&ast_lambda);
	expr->m1.mem = context_create(*tos, (MEMORY *)0);
}

/* Primitive functions (e.g. $iadd) use these to retrieve atomic parameter
** values from the stack. Requires the primitive function to be used in
** combination with $strict. See unop and binop in fblrt.fbl.
*/
static MEMORY *strict_get_argument(MEMORY *expr)
{
	NODE *node = get_arg(expr);
	assert(node->nodetype == NT_IDENT);
	return context_get(expr->m1.mem, AS_IDENT(node)->index)->m0.mem;
}

long strict_get_long(MEMORY *expr)
{
	MEMORY *arg = strict_get_argument(expr);
	assert(arg->m0.ast == &expr_int);
	return arg->m1.i;
}

double strict_get_double(MEMORY *expr)
{
	MEMORY *arg = strict_get_argument(expr);
	assert(arg->m0.ast == &expr_double);
	return arg->m1.d;
}
