#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int min = 0, max = 0, n = 0;
FILE* outF;

void writeDir(DIR *dr, int currDir, char* dirStr)
{
	errno = 0;
	struct dirent *d = readdir(dr);
	while(d != NULL)
	{
		int Fd = openat(currDir, d->d_name, O_RDONLY);
		struct stat st1; 
		if(fstat(Fd, &st1) == -1)
		{
			perror("fstat");
			exit(EXIT_FAILURE);
		}
	
		if(S_ISDIR(st1.st_mode))
		{
			DIR *dr1 = fdopendir(Fd);
			if(dr == NULL)
			{
				perror("opendir");
				exit(EXIT_FAILURE);
			}
			
			int tempLen = strlen(dirStr);
			if(dirStr[tempLen-1] != '/')
				tempLen++;
			char *newDir = (char*)malloc(tempLen + strlen(d->d_name) + 1);
			strcpy(newDir, dirStr);
			newDir[tempLen] = 0;
			newDir[tempLen - 1] = '/';
			strcat(newDir, d->d_name);
			//fprintf(stdout, "%s\n", newDir);
			
			if (strncmp(d->d_name, ".", 1) != 0 && strncmp(d->d_name, "..", 2) != 0)
				writeDir(dr1, Fd, newDir);
			
			free(newDir);
			
			if(closedir(dr1) == -1)
			{
				perror("closedir");
				exit(EXIT_FAILURE);
			}
		}	
		else
		{
			if(st1.st_size > min && st1.st_size < max)
			{
				fprintf(outF, "Path: %s; Name: %s; Size: %ld\n", dirStr, d->d_name, st1.st_size);
				n++;
			}	
			if(close(Fd) == -1)
			{
				perror("close");
				exit(EXIT_FAILURE);
			}	
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
	if(argc != 5)
	{
		fprintf(stderr, "Программа требует 4 аргумента.\n");
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
	min = val;
	
	errno = 0;
	str = argv[3];
	val = strtol(str, &endptr, 0);
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
	max = val;
	
	DIR *dr = opendir(argv[1]);
	if(dr == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	int F = dirfd(dr);
	if(F == -1)
	{
		perror("dirfd");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "|---------------------|\n");
	fprintf(stdout, "Корневой каталог:\n");
	n = 0;
	
	outF = fopen(argv[4], "w");
	if(outF == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	writeDir(dr, F, argv[1]);
	
	fprintf(stdout, "Файлов найдено: %d\n", n);
	
	if(closedir(dr) == -1)
	{
		perror("closedir");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
