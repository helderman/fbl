/* heap.h - memory cells */

typedef struct memory MEMORY;

struct memory {
	union m0 {
		MEMORY *mem;
		NODE *ast;
	} m0;
	union m1 {
		MEMORY *mem;
		long i;
		double d;
	} m1;
};

extern int     heap_size;

extern void    heap_init(void);
extern MEMORY *heap_alloc(MEMORY *ptr0, MEMORY *ptr1);
extern void    heap_free(MEMORY *node);
