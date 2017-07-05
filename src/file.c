/* file.c - read FBL source files
**
** Used by the tokenizer to retrieve a continuous stream of characters,
** made up of the contents of all specified source files together.
**
** Automatically opens and closes each file.
** Calls arg_get_filename whenever it needs another file to read
** (i.e. at the start, and whenever the current file reaches EOF).
**
** Please note that characters are returned as if the files are concatenated.
** You are advised to end every source file with a newline (UNIX style),
** otherwise our tokenizer will behave as if the first line of a file was
** appended to the last line of the file before (if any),
** which may not be what you expected.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg.h"

static FILE *file_ptr = NULL;
static int file_char = 0;

const char *file_name = NULL;
int file_line = 0;
int file_column = 0;

static int file_open(void)
{
	if (file_ptr == NULL)
	{
		file_name = arg_get_filename();
		if (file_name != NULL)
		{
			file_line = 1;
			file_column = 0;
			file_ptr = strcmp(file_name, "-") == 0 ? stdin : fopen(file_name, "rt");
			if (file_ptr == NULL)
			{
				fprintf(stderr, "Cannot open source file '%s'.\n", file_name);
				exit(1);
			}
		}
	}
	file_char = EOF;
	return file_ptr != NULL;
}

static int file_at_eof(void)
{
	file_char = getc(file_ptr);
	if (file_char == '\n')
	{
		file_line++;
		file_column = 0;
	}
	else if (file_char != EOF)
	{
		file_column++;
	}
	return file_char == EOF;
}

int file_getc(void)
{
	if (file_char != EOF)
	{
		while (file_open() && file_at_eof())
		{
			if (file_ptr != stdin) fclose(file_ptr);
			file_ptr = NULL;
		}
	}
	return file_char;
}
