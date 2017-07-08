/* strict.h - strict evaluation */

extern void   strict_reduction(MEMORY **tos, MEMORY *expr);
extern long   strict_get_long(MEMORY *expr);
extern double strict_get_double(MEMORY *expr);
