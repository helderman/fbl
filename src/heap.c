/* heap.c - memory cells */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "heap.h"
#include "stack.h"
#include "trace.h"

#define MARK(p)         ((MEMORY *)((char *)(p) + 1))
#define UNMARK(p)       ((MEMORY *)((char *)(p) - 1))
#define IS_MARKED(p)    ((int)(p) & 1)

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
			assert(IS_MARKED(current->m0.mem));
			dest = current->m1.mem;
			if (IS_MARKED(dest))
			{
				/* Back from m1: move further backward */
				current->m1.mem = back;
			}
			else
			{
				/* Back from m0: move forward to m1 */
				current->m1.mem = current->m0.mem;
				current->m0.mem = MARK(back);
			}
			back = current;
		}
		else if (current == NULL || IS_MARKED(current->m0.mem))
		{
			/* Dead end; go back */
			dest = MARK(back);
			back = current;
		}
		else
		{
			/* Moving forward into an unmarked cell.
			** Mark m0, and move forward into m0 (expr).
			*/
			assert(!IS_MARKED(current->m1.mem));
			dest = current->m0.mem;
			current->m0.mem = MARK(back);
			++count_marked;
			back = current;
			if (dest != NULL && !IS_MARKED(dest->m0.mem))
			{
				NODE *node = dest->m0.ast;
				assert(node != NULL);
				dest->m0.mem = MARK(dest->m0.mem);
				++count_marked;
				current = dest->m1.mem;
				if (current != NULL && node->nodetype != NT_ATOM)
				{
					dest->m1.mem = MARK(back);
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
		if (IS_MARKED(node->m0.mem))
		{
			node->m0.mem = UNMARK(node->m0.mem);
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
	freelist = mine->m0.mem;
	mine->m0.mem = ptr0;
	mine->m1.mem = ptr1;
	++heap_count_alloc;
	return mine;
}

/* Free a node by adding it to the free list.
** Only use this when you are absolutely sure
** there are no more references to this node!
*/
void heap_free(MEMORY *node)
{
	node->m0.mem = freelist;
	freelist = node;
}
