/* token.c - tokenizer */

#include <ctype.h>
#include <stdio.h>

#include "file.h"
#include "token.h"

#define MAXLEN_IDENT	64

#define IS_SIMPLE_TOKEN(c) \
	((c) == TOKEN_EOF || \
	 (c) == TOKEN_OPEN || (c) == TOKEN_CLOSE || \
	 (c) == TOKEN_WHERE || (c) == TOKEN_LAMBDA)

char token_buffer[MAXLEN_IDENT + 1];
char token_char;
long token_int;
double token_double;

static int skip_whitespace(int c)
{
	int comment = 0;
	while (c != EOF && (isspace(c) || (comment |= c == '#')))
	{
		if (c == '\n') comment = 0;
		c = file_getc();
	}
	return c;
}

static int is_ident(const char *p)
{
	int ok;
	for (ok = !isdigit(*p); ok && *p != '\0'; p++)
	{
		ok = *p == '_' || isalnum(*p);
	}
	return ok;
}

static int is_number(const char *p, int len, const char *format, void *dest)
{
	int scanned = 0;
	return sscanf(p, format, dest, &scanned) > 0 && scanned == len;
}

static int get_char_literal(int c, char *dest)
{
	if (c != '\n')
	{
		if (c == '\\')
		{
			do {c = file_getc(); } while (c == '\n');
			switch (c)
			{
				case '0': c = '\0'; break;
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				default : break;
			}
		}
		if (c != EOF)
		{
			*dest = (char)c;
			c = file_getc();	/* expected closing quote */
		}
	}
	return c;
}

static int get_string_literal(void)
{
	char *ptr = token_buffer;
	int c = file_getc();
	while (c != EOF && c != '\n' && c != '"')
	{
		c = get_char_literal(c, ptr++);
	}
	*ptr = '\0';
	return c;
}

TOKEN token_get(void)
{
	static int c = ' ';

	TOKEN token;
	c = skip_whitespace(c);
	if (IS_SIMPLE_TOKEN(c))
	{
		token = c;
		if (c != EOF) c = file_getc();
	}
	else if (c == '\'')
	{
		c = get_char_literal(file_getc(), &token_char);
		token = c == '\'' ? TOKEN_CHAR : TOKEN_ERROR;
		if (c != EOF) c = file_getc();
	}
	else if (c == '"')
	{
		c = get_string_literal();
		token = c == '"' ? TOKEN_STRING : TOKEN_ERROR;
		if (c != EOF) c = file_getc();
	}
	else
	{
		int i = 0;
		while (!IS_SIMPLE_TOKEN(c) && !isspace(c))
		{
			if (i < MAXLEN_IDENT) token_buffer[i++] = c;
			c = file_getc();
		}
		token_buffer[i] = '\0';
		token =
			is_number(token_buffer, i, "%ld%n", &token_int) ?
				TOKEN_INT :
			is_number(token_buffer, i, "%lf%n", &token_double) ?
				TOKEN_DOUBLE :
			*token_buffer == '$' && is_ident(token_buffer + 1) ?
				TOKEN_PRIM :
			is_ident(token_buffer) ?
				TOKEN_IDENT : TOKEN_ERROR;
	}
	return token;
}
