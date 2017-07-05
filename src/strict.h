/* strict.h - strict evaluation */

extern void   strict_reduction(EXPR **tos, EXPR *expr);
extern long   strict_get_long(EXPR *expr);
extern double strict_get_double(EXPR *expr);
