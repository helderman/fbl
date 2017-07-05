/* stack.h - execution stack */

typedef struct stack {
	EXPR *expr;
	struct stack *next;
} STACK;

extern int    stack_not_empty(void);
extern EXPR  *stack_tos(void);
extern EXPR  *stack_peek(void);
extern EXPR **stack_push(void);
extern EXPR  *stack_pop(void);
extern void   stack_pop_push(EXPR *expr);

extern STACK *stack_pointer(void);

/* hooks in case you want to experiment with reference counts in GC */
#define stack_unlink(stack) (void)(stack)
