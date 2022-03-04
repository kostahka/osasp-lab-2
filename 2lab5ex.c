#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

void main(int argc, char *argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Программа требует 2 аргумента.\n");
		exit(EXIT_FAILURE);
	}
	FILE *F1 = fopen(argv[1], "r");
	if(F1 == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}	
	
	FILE *F2 = fopen(argv[2], "w");
	if(F2 == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	char c = 0;
	while (c != EOF)
	{
		errno = 0;
		c = fgetc(F1);
		if (c == EOF && errno != 0)
		{
			perror("fgetc");
			exit(EXIT_FAILURE);
		}
		else if(c != EOF)
		{
			if(fputc(c, F2) == EOF)
			{
				fprintf(stderr, "Ошибка записи символа.\n");
				//perror("fputc");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	if (fclose(F1) == EOF)
	{
		perror("fclose");
		exit(EXIT_FAILURE);
	}
	
	if (fclose(F2) == EOF)
	{
		perror("fclose");
		exit(EXIT_FAILURE);
	}
	
	struct stat st1; 
	if(stat(argv[1], &st1) == -1)
	{
		perror("fstat");
		exit(EXIT_FAILURE);
	}
	
	if(chmod(argv[2], st1.st_mode) == -1)
	{
		perror("chmod");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);	
}
