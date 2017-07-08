/* stack.h - execution stack */

extern int      stack_not_empty(void);
extern MEMORY  *stack_tos(void);
extern MEMORY  *stack_peek(void);
extern MEMORY **stack_push(void);
extern MEMORY  *stack_pop(void);
extern void     stack_pop_push(MEMORY *expr);

extern MEMORY  *stack_pointer(void);

/* hooks in case you want to experiment with reference counts in GC */
#define stack_unlink(stack) (void)(stack)
