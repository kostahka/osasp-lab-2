#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

char printLine(FILE *F)
{
	errno = 0;
	char c = 0;
	while(c != '\n')
	{
		c = fgetc(F);
		if(c == EOF && errno == 0)
		{
			return c;
		}
		else if(errno != 0)
		{
			perror("fgetc");
			exit(EXIT_FAILURE);
		}
		if(c != '\r')
			fputc(c, stdout);
	}
	return c;
}

void main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Программа требует 2 аргумента.\n");
		exit(EXIT_FAILURE);
	}
	errno = 0;
	char *str, *endptr;
	str = argv[2];
	long val = strtol(str, &endptr, 0);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
	{
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	if (endptr == str)
	{
		fprintf(stderr, "No digits were found\n");
		exit(EXIT_FAILURE);
	}
	
	FILE *F1 = fopen(argv[1], "r");
	if (F1 == NULL){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char c = 0;
	if(val == 0)
	{
		while(c != EOF)
		{
			c = printLine(F1);
		}
	}
	else
	{
		int i = val;
		while(c != EOF && i > 0)
		{
			c = printLine(F1);
			i--;
		}
		while(c != EOF)
		{
			fprintf(stdout, "Print any character to continue...");
			if (getchar() == EOF)
			{
				perror("getc");
				exit(EXIT_FAILURE);
			}
			int i = val;
			while(c != EOF && i > 0)
			{
				c = printLine(F1);
				i--;
			}
		}
	}
	fprintf(stdout, "Thats all...\n");
	exit(EXIT_SUCCESS);
}
