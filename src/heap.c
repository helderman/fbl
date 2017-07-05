/* heap.c - memory cells */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "expr.h"
#include "context.h"
#include "stack.h"
#include "heap.h"
#include "trace.h"

#define MARK(p)         ((MEMORY *)((char *)(p) + 1))
#define UNMARK(p)       ((MEMORY *)((char *)(p) - 1))
#define IS_MARKED(p)    ((int)(p) & 1)

union memory {
	union memory *ptr[2];
	EXPR expr;
	CONTEXT context;
	STACK stack;
};

static MEMORY *memory, *freelist;
static int heap_count_alloc = 0;

void heap_init(void)
{
	int count = heap_size;
	memory = malloc(count * sizeof *memory);
	freelist = NULL;
	while (--count >= 0)
	{
		heap_free(memory + count);
	}
}

static int heap_gc_mark(MEMORY *dest)
{
	int count_marked = 0;
	MEMORY *back = NULL;
	while (dest != MARK(NULL))
	{
		MEMORY *current = dest;
		assert(!IS_MARKED(back));
		if (IS_MARKED(current))
		{
			/* Moving backward into a marked cell.
			*/
			current = UNMARK(current);
			assert(IS_MARKED(current->ptr[0]));
			dest = current->ptr[1];
			if (IS_MARKED(dest))
			{
				/* Back from ptr[1]: move further backward */
				current->ptr[1] = back;
			}
			else
			{
				/* Back from ptr[0]: move forward to ptr[1] */
				current->ptr[1] = current->ptr[0];
				current->ptr[0] = MARK(back);
			}
			back = current;
		}
		else if (current == NULL || IS_MARKED(current->ptr[0]))
		{
			/* Dead end; go back */
			dest = MARK(back);
			back = current;
		}
		else
		{
			/* Moving forward into an unmarked cell.
			** Mark ptr[0], and move forward into ptr[0] (expr).
			*/
			assert(!IS_MARKED(current->ptr[1]));
			dest = current->ptr[0];
			current->ptr[0] = MARK(back);
			++count_marked;
			back = current;
			if (dest != NULL && !IS_MARKED(dest->ptr[0]))
			{
				NODE *node = ((EXPR *)dest)->ast;
				assert(node != NULL);
				dest->ptr[0] = MARK(dest->ptr[0]);
				++count_marked;
				current = dest->ptr[1];
				if (current != NULL && node->nodetype != NT_ATOM)
				{
					dest->ptr[1] = MARK(back);
					back = dest;
					dest = current;
				}
			}
			assert(!IS_MARKED(dest));
		}
	}
	return count_marked;
}

static int heap_gc_sweep(void)
{
	int collected = 0;
	int count = heap_size;
	while (--count >= 0)
	{
		MEMORY *node = memory + count;
		if (IS_MARKED(node->ptr[0]))
		{
			node->ptr[0] = UNMARK(node->ptr[0]);
		}
		else
		{
			heap_free(node);
			++collected;
		}
	}
	return collected;
}

static MEMORY *heap_gc(void)
{
#ifdef TRACE
	++trace_count_gc;
#endif
	heap_gc_mark((MEMORY *)stack_pointer());
	heap_gc_sweep();
	return freelist;
}

MEMORY *heap_alloc(MEMORY *ptr0, MEMORY *ptr1)
{
	MEMORY *mine;
	if (freelist == NULL && heap_gc() == NULL)
	{
		fprintf(stderr, "FBL runtime out of heap space.\n");
		exit(1);
	}
	mine = freelist;
	freelist = mine->ptr[0];
	mine->ptr[0] = ptr0;
	mine->ptr[1] = ptr1;
	++heap_count_alloc;
	return mine;
}

/* Free a node by adding it to the free list.
** Only use this when you are absolutely sure
** there are no more references to this node!
*/
void heap_free(MEMORY *node)
{
	node->ptr[0] = freelist;
	freelist = node;
}
