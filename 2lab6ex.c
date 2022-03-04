#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>

void writeDir(DIR *dr)
{
	errno = 0;
	struct dirent *d = readdir(dr);
	while(d != NULL)
	{
		for(int i = 0; d->d_name[i] != 0 && i < MAXNAMLEN; i++)
		{
			if(fputc(d->d_name[i], stdout) == EOF)
			{
				fprintf(stderr, "Ошибка записи символа.\n");
				exit(EXIT_FAILURE);
			}
		}
		if(fputc('\n', stdout) == EOF)
		{
			fprintf(stderr, "Ошибка записи символа.\n");
			exit(EXIT_FAILURE);
		}
		d = readdir(dr);
	}
	
	if(errno != 0)
	{
		perror("readdir");
		exit(EXIT_FAILURE);
	}
	return;
}

void main(int argc, char *argv[])
{
	if(argc != 1)
	{
		fprintf(stderr, "Программа не требует аргумента.\n");
		exit(EXIT_FAILURE);
	}
	
	DIR *dr = opendir("./");
	if(dr == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "Текущий каталог:\n");
	writeDir(dr);
	
	dr = opendir("/");
	if(dr == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "|---------------------|\n");
	fprintf(stdout, "Корневой каталог:\n");
	writeDir(dr);
	
	exit(EXIT_SUCCESS);
}
