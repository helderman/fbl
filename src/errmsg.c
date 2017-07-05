/* errmsg.c - build parser error message */

#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "token.h"

static void part(FILE *fp, TOKEN token)
{
	switch (token)
	{
	case TOKEN_EOF:
		fprintf(fp, "EOF");
		break;
	case TOKEN_ERROR:
		fprintf(fp, "lexical error");
		break;
	case TOKEN_IDENT:
		fprintf(fp, "identifier");
		break;
	case TOKEN_CHAR:
		fprintf(fp, "char literal");
		break;
	case TOKEN_INT:
	case TOKEN_DOUBLE:
		fprintf(fp, "numeric literal");
		break;
	case TOKEN_STRING:
		fprintf(fp, "string literal");
		break;
	default:
		fprintf(fp, "'%c'", token);
		break;
	}
}

void error_message(FILE *fp, TOKEN expected, TOKEN found, TOKEN previous)
{
	fprintf(fp, "[%s:%d:%d] Expected ", file_name, file_line, file_column);
	switch (expected)
	{
	case TOKEN_OPEN:
		fprintf(fp, "atomic expression");
		break;
	case TOKEN_WHERE:
		fprintf(fp, "expression");
		break;
	default:
		part(fp, expected);
		break;
	}
	fprintf(fp, " instead of ");
	part(fp, found);
	if (previous != TOKEN_EOF)
	{
		fprintf(fp, " after ");
		part(fp, previous);
	}
	fprintf(fp, ".\n");
	exit(1);
}
