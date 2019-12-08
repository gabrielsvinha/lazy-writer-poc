#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "io_interface.h"

typedef struct
{
	char *type;
	char *path;
	int size;
} operation;

char **str_split(char *a_str, const char a_delim)
{
	char **result = 0;
	size_t count = 0;
	char *tmp = a_str;
	char *last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp)
	{
		if (a_delim == *tmp)
		{
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += last_comma < (a_str + strlen(a_str) - 1);

	/* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
	count++;

	result = malloc(sizeof(char *) * count);

	if (result)
	{
		size_t idx = 0;
		char *token = strtok(a_str, delim);

		while (token)
		{
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}

	return result;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("%s: operations file required\n", argv[0]);
		return 1;
	}
	const char *r = "r";
	const char *w = "w";
	int rc = setup_context();
	if (rc)
	{
		printf("unable to init context, is liburing installed?");
		return 1;
	}
	FILE *stream = fopen(argv[1], "r");
	char line[1024];
	while (fgets(line, 1024, stream))
	{
		char *tmp = strdup(line);
		// printf("Field 3 would be %s\n", getfield(tmp, 3));
		char **tokens = str_split(tmp, ',');
		operation* op = {&tokens[0], &tokens[1], (int)&tokens[2]};
		if (strcmp(op->type, w) == 0)
		{
			write_file(op->path, op->size);
		}
		else if (strcmp(op->type, r) == 0)
		{
			read_file(op->path);
		}
		
		free(tmp);
	}
 
	return 0;
}