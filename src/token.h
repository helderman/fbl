/* token.h - tokenizer */

typedef enum {
	TOKEN_EOF    = EOF,
	TOKEN_ERROR  = 0,
	TOKEN_PRIM   = 1,
	TOKEN_IDENT  = 2,
	TOKEN_CHAR   = 3,
	TOKEN_INT    = 4,
	TOKEN_DOUBLE = 5,
	TOKEN_STRING = 6,
	TOKEN_OPEN   = '(',
	TOKEN_CLOSE  = ')',
	TOKEN_WHERE  = ';',
	TOKEN_LAMBDA = '\\'
} TOKEN;

extern char token_buffer[];
extern char token_char;
extern long token_int;
extern double token_double;

extern TOKEN token_get(void);
