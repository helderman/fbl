/* parse.c - parser
**
** Syntax:
**   <expression> ::= <apply>
**                  | <expression> ";" <identifier> <atomic>
**   <apply>      ::= <atomic>
**                  | <apply> <atomic>
**   <atomic>     ::= "\" <identifier> <atomic>
**                  | "(" <expression> ")"
**                  | <identifier>
**                  | <literal>
**
** The backslash ('\') starts a lambda expression (similar to Haskell).
** The semicolon (';') is a recursive let operator, but with reversed notation
** (the definitions come last; like 'where' in Haskell).
**
** Example:    f 10 20 ; f \x \y (g x y) ; g \x \y (f x y)
**
** This should apply function f on two numeric arguments (literals),
** in a context where f and g are defined as mutually recursive functions.
** Syntactically sound, semantically an infinite loop.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define AST_NODE_ID
#ifndef AST_NAMES
#define AST_NAMES
#endif

#include "ast.h"
#include "token.h"
#include "errmsg.h"

#define STARTS_ATOMIC(token)  \
	((token) > TOKEN_ERROR && (token) != TOKEN_CLOSE && (token) != TOKEN_WHERE)

static NODE *parse_expression(TOKEN expected);

static NODE *parse_atomic(TOKEN token)
{
	NODE *ast = NULL;
	switch (token)
	{
	case TOKEN_LAMBDA:
		token = token_get();
		if (token == TOKEN_IDENT)
		{
			const char *name = strdup(token_buffer);
			ast = create_lambda(parse_atomic(token_get()), name);
		}
		else
		{
			error_message(stderr, TOKEN_IDENT, token, TOKEN_LAMBDA);
		}
		break;
	case TOKEN_PRIM:
		ast = create_prim(strdup(token_buffer + 1));
		break;
	case TOKEN_IDENT:
		ast = create_ident(strdup(token_buffer));
		break;
	case TOKEN_CHAR:
		ast = create_int((long)token_char);
		break;
	case TOKEN_INT:
		ast = create_int(token_int);
		break;
	case TOKEN_DOUBLE:
		ast = create_double(token_double);
		break;
	case TOKEN_STRING:
		ast = create_string(token_buffer);
		break;
	case TOKEN_OPEN:
		ast = parse_expression(TOKEN_CLOSE);
		break;
	default:
		error_message(stderr, TOKEN_OPEN, token, TOKEN_EOF);
		break;
	}
	return ast;
}

static DEFINE *parse_defines(TOKEN token, TOKEN expected)
{
	DEFINE *defines = NULL;
	while (token == TOKEN_WHERE)
	{
		token = token_get();
		if (token == TOKEN_IDENT)
		{
			const char *name = strdup(token_buffer);
			defines = create_define(defines, parse_atomic(token_get()), name);
		}
		else
		{
			error_message(stderr, TOKEN_IDENT, token, TOKEN_WHERE);
		}
		token = token_get();
	}
	if (token != expected)
	{
		error_message(stderr, expected, token, TOKEN_EOF);
	}
	return defines;
}

static NODE *parse_expression(TOKEN expected)
{
	NODE *ast = NULL;
	int token = token_get();
	if (!STARTS_ATOMIC(token))
	{
		error_message(stderr, TOKEN_WHERE, token, TOKEN_EOF);
	}
	else
	{
		ast = parse_atomic(token);
		token = token_get();
		while (STARTS_ATOMIC(token))
		{
			ast = create_apply(ast, parse_atomic(token));
			token = token_get();
		}
		ast = create_rlet(ast, parse_defines(token, expected));
	}
	return ast;
}

DEFINE *parse_module(void)
{
	return parse_defines(token_get(), TOKEN_EOF);
}

NODE *parse_root(void)
{
	return parse_expression(TOKEN_EOF);
}
