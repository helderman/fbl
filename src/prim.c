/* prim.c - primitive functions */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "heap.h"
#include "expr.h"
#include "enum.h"
#include "strict.h"
#include "stack.h"
#include "trace.h"

#ifdef AST_NAMES
#define initializer(name)  {NT_PRIM, reduce_##name, #name}
#else
#define initializer(name)  {NT_PRIM, reduce_##name}
#endif

static long pop_long(void)
{
	MEMORY *expr = stack_pop();
	long value = strict_get_long(expr);
	expr_unlink(expr);
	return value;
}

static double pop_double(void)
{
	MEMORY *expr = stack_pop();
	double value = strict_get_double(expr);
	expr_unlink(expr);
	return value;
}

static void reduce_strict(void)
{
	MEMORY *expr = stack_tos();
	strict_reduction(stack_push(), expr);
}

static void reduce_error(void)
{
	fprintf(stderr, "Evaluation hit error primitive.\n");
	exit(1);
}

#define UNOP(out_func, in_type, formula)	\
	MEMORY *expr = stack_tos();		\
	in_type x = strict_get_##in_type(expr);	\
	out_func(expr, formula);

#define BINOP(out_func, in_type, formula)	\
	in_type x = pop_##in_type();		\
	MEMORY *expr = stack_tos();		\
	in_type y = strict_get_##in_type(expr);	\
	out_func(expr, formula);

#define DBL_UNARY(formula)      UNOP(expr_to_double, double, formula)
#define INT_ARITHMETIC(formula) BINOP(expr_to_int, long, formula)
#define DBL_ARITHMETIC(formula) BINOP(expr_to_double, double, formula)
#define INT_RELATIONAL(formula) BINOP(expr_to_enum, long, formula)
#define DBL_RELATIONAL(formula) BINOP(expr_to_enum, double, formula)

static long iquot(long x, long y)
{
	return y<0 ? (x>0 ? -(-x / y) : x / y) : (x<0 ? -(x / -y) : x / y);
}

static long irem(long x, long y)
{
	return x - iquot(x, y) * y;
}

static long idiv(long x, long y)
{
	return y<0 ? (x>0 ? ~((1-x) / y) : x / y) : (x<0 ? ~(~x / y) : x / y);
}

static long imod(long x, long y)
{
	return x - idiv(x, y) * y;
}

static void reduce_dtoint(void)    { UNOP(expr_to_int, double, (long)x) }
static void reduce_itodbl(void)    { UNOP(expr_to_double, long, (double)x) }

static void reduce_iadd(void)      { INT_ARITHMETIC(x + y) }
static void reduce_isub(void)      { INT_ARITHMETIC(x - y) }
static void reduce_imul(void)      { INT_ARITHMETIC(x * y) }
static void reduce_iquot(void)     { INT_ARITHMETIC(iquot(x, y)) }
static void reduce_irem(void)      { INT_ARITHMETIC(irem(x, y)) }
static void reduce_idiv(void)      { INT_ARITHMETIC(idiv(x, y)) }
static void reduce_imod(void)      { INT_ARITHMETIC(imod(x, y)) }
static void reduce_iand(void)      { INT_ARITHMETIC(x & y) }
static void reduce_ior(void)       { INT_ARITHMETIC(x | y) }
static void reduce_ixor(void)      { INT_ARITHMETIC(x ^ y) }
static void reduce_icompare(void)  { INT_RELATIONAL(ENUM_ORDERING(x - y)) }

static void reduce_dadd(void)      { DBL_ARITHMETIC(x + y) }
static void reduce_dsub(void)      { DBL_ARITHMETIC(x - y) }
static void reduce_dmul(void)      { DBL_ARITHMETIC(x * y) }
static void reduce_ddiv(void)      { DBL_ARITHMETIC(x / y) }
static void reduce_dcompare(void)  { DBL_RELATIONAL(ENUM_ORDERING(x - y)) }

static void reduce_exp(void)       { DBL_UNARY(exp(x)) }
static void reduce_exp2(void)      { DBL_UNARY(exp2(x)) }
static void reduce_log(void)       { DBL_UNARY(log(x)) }
static void reduce_log2(void)      { DBL_UNARY(log2(x)) }
static void reduce_log10(void)     { DBL_UNARY(log10(x)) }
static void reduce_sqrt(void)      { DBL_UNARY(sqrt(x)) }
static void reduce_sin(void)       { DBL_UNARY(sin(x)) }
static void reduce_cos(void)       { DBL_UNARY(cos(x)) }
static void reduce_tan(void)       { DBL_UNARY(tan(x)) }
static void reduce_asin(void)      { DBL_UNARY(asin(x)) }
static void reduce_acos(void)      { DBL_UNARY(acos(x)) }
static void reduce_atan(void)      { DBL_UNARY(atan(x)) }
static void reduce_sinh(void)      { DBL_UNARY(sinh(x)) }
static void reduce_cosh(void)      { DBL_UNARY(cosh(x)) }
static void reduce_tanh(void)      { DBL_UNARY(tanh(x)) }
static void reduce_asinh(void)     { DBL_UNARY(asinh(x)) }
static void reduce_acosh(void)     { DBL_UNARY(acosh(x)) }
static void reduce_atanh(void)     { DBL_UNARY(atanh(x)) }
static void reduce_ceil(void)      { DBL_UNARY(ceil(x)) }
static void reduce_floor(void)     { DBL_UNARY(floor(x)) }
static void reduce_trunc(void)     { DBL_UNARY(trunc(x)) }
static void reduce_round(void)     { DBL_UNARY(round(x)) }

static long stdin_position = 0;

static void reduce_getc(void)
{
	MEMORY *expr = stack_tos();
	int result = strict_get_long(expr) == stdin_position ? getchar() : EOF;
	if (result != EOF)
	{
		stdin_position++;
	}
	expr_to_int(expr, result);
}

static void reduce_putc(void)
{
	MEMORY *expr = stack_tos();
	expr_to_int(expr, putchar((int)strict_get_long(expr)));
}

static void reduce_true(void)
{
	MEMORY **tos = stack_push();
	*tos = expr_create(AS_NODE(ENUM_BOOLEAN(1)), NULL);
}

static void reduce_false(void)
{
	MEMORY **tos = stack_push();
	*tos = expr_create(AS_NODE(ENUM_BOOLEAN(0)), NULL);
}

static void reduce_lt(void)
{
	MEMORY **tos = stack_push();
	*tos = expr_create(AS_NODE(ENUM_ORDERING(-1)), NULL);
}

static void reduce_eq(void)
{
	MEMORY **tos = stack_push();
	*tos = expr_create(AS_NODE(ENUM_ORDERING(0)), NULL);
}

static void reduce_gt(void)
{
	MEMORY **tos = stack_push();
	*tos = expr_create(AS_NODE(ENUM_ORDERING(1)), NULL);
}

NODE_PRIM ast_strict	= initializer(strict);
NODE_PRIM ast_error	= initializer(error);
NODE_PRIM ast_dtoint	= initializer(dtoint);
NODE_PRIM ast_itodbl	= initializer(itodbl);
NODE_PRIM ast_iadd	= initializer(iadd);
NODE_PRIM ast_isub	= initializer(isub);
NODE_PRIM ast_imul	= initializer(imul);
NODE_PRIM ast_iquot	= initializer(iquot);
NODE_PRIM ast_irem	= initializer(irem);
NODE_PRIM ast_idiv	= initializer(idiv);
NODE_PRIM ast_imod	= initializer(imod);
NODE_PRIM ast_iand	= initializer(iand);
NODE_PRIM ast_ior	= initializer(ior);
NODE_PRIM ast_ixor	= initializer(ixor);
NODE_PRIM ast_icompare	= initializer(icompare);
NODE_PRIM ast_dadd	= initializer(dadd);
NODE_PRIM ast_dsub	= initializer(dsub);
NODE_PRIM ast_dmul	= initializer(dmul);
NODE_PRIM ast_ddiv	= initializer(ddiv);
NODE_PRIM ast_dcompare	= initializer(dcompare);
NODE_PRIM ast_exp	= initializer(exp);
NODE_PRIM ast_exp2	= initializer(exp2);
NODE_PRIM ast_log	= initializer(log);
NODE_PRIM ast_log2	= initializer(log2);
NODE_PRIM ast_log10	= initializer(log10);
NODE_PRIM ast_sqrt	= initializer(sqrt);
NODE_PRIM ast_sin	= initializer(sin);
NODE_PRIM ast_cos	= initializer(cos);
NODE_PRIM ast_tan	= initializer(tan);
NODE_PRIM ast_asin	= initializer(asin);
NODE_PRIM ast_acos	= initializer(acos);
NODE_PRIM ast_atan	= initializer(atan);
NODE_PRIM ast_sinh	= initializer(sinh);
NODE_PRIM ast_cosh	= initializer(cosh);
NODE_PRIM ast_tanh	= initializer(tanh);
NODE_PRIM ast_asinh	= initializer(asinh);
NODE_PRIM ast_acosh	= initializer(acosh);
NODE_PRIM ast_atanh	= initializer(atanh);
NODE_PRIM ast_ceil	= initializer(ceil);
NODE_PRIM ast_floor	= initializer(floor);
NODE_PRIM ast_trunc	= initializer(trunc);
NODE_PRIM ast_round	= initializer(round);
NODE_PRIM ast_getc	= initializer(getc);
NODE_PRIM ast_putc	= initializer(putc);
NODE_PRIM ast_true	= initializer(true);
NODE_PRIM ast_false	= initializer(false);
NODE_PRIM ast_lt  	= initializer(lt);
NODE_PRIM ast_eq  	= initializer(eq);
NODE_PRIM ast_gt  	= initializer(gt);
