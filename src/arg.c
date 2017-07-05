/* arg.c - command line arguments
**
** Usage:
**   arg_init(argv);
**   while ((filename = arg_get_filename()) != NULL) { ... }
**   heapsize = arg_heapsize;
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static char **arg_current;

/* Number of cells in heap.
** Default is 16000; can be overridden with command line option -h<number>.
** Because this option may appear anywhere in the command line
** (and filenames and options are parsed in a single pass),
** the value of arg_heapsize should be considered undefined
** until after the last call to arg_get_filename (the one that returns NULL).
*/
int arg_heapsize = 16000;

/* Initialize pointer to argument list.
** Set the pointer to &argv[0]; function arg_parse_option uses pre-increment,
** so it will skip argv[0] (the executable) and start parsing at argv[1].
*/
void arg_init(char **args)
{
	arg_current = args;
}

/* Parse a single command line option.
** Currently only recognizes -h<number> (no separating space allowed).
** Any other option will cause the program to stop with a usage message.
** If an option is specified more than once, then the last one wins.
*/
static void arg_parse_option(const char *s)
{
	int num;
	if (sscanf(s, "h%d", &num) == 1 && num > 0)
	{
		arg_heapsize = num;
	}
	else
	{
		fprintf(stderr,
			"FBL compiler - Ruud Helderman 2014-2016\n"
			"Usage: fblc [-h<heap>] <filename> ...\n");
		exit(1);
	}
}

/* Advance the pointer to the next filename in the argument list.
** Returns either a pointer to the filename found, or NULL if no more found.
** A single dash ('-') is considered a filename.
** Anything else starting with a dash is NOT considered a filename;
** it is skipped, and interpreted by function arg_parse_option (above).
*/
const char *arg_get_filename(void)
{
	const char *s;
	assert(*arg_current != NULL);	/* must not go beyond last argument */
	while ((s = *++arg_current) != NULL && s[0] == '-' && s[1] != '\0')
	{
		arg_parse_option(s + 1);
	}
	return s;
}
