/* heap.h - memory cells */

typedef union memory MEMORY;

extern int     heap_size;

extern void    heap_init(void);
extern MEMORY *heap_alloc(MEMORY *ptr0, MEMORY *ptr1);
extern void    heap_free(MEMORY *node);
