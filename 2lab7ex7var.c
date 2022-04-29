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

int min = 0, max = 0;

struct FileNode
{
	char* name;
	struct FileNode* next;
	int size;
};

struct SimilarFile
{
	char* name;
	int n;
	struct SimilarFile* next;
	struct FileNode* first;
};

int CheckForSimillar(char* name1, char* name2){
	FILE* F1 = fopen(name1, "r");
	if(F1 == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	FILE* F2 = fopen(name2, "r");
	if(F2 == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	int c1 = getc(F1), c2 = getc(F2);
	while(c1 == c2 && c1 != EOF && c2 != EOF)
	{
		c1 = getc(F1);
		c2 = getc(F2);
	}
	
	if(fclose(F1) == EOF)
	{
		perror("fclose");
		exit(EXIT_FAILURE);
	}
	if(fclose(F2) == EOF)
	{
		perror("fclose");
		exit(EXIT_FAILURE);
	}
	
	
	if(c1 == c2)
		return 1;
	else
		return 0;
	
}

void AddDir(struct SimilarFile* file, char* name, int size)
{
	if(file->first == NULL)
	{
		file->first = (struct FileNode*) malloc(sizeof(struct FileNode));
		struct FileNode* temp = file->first;
		temp->name = name;
		temp->next = NULL;
		temp->size = size;
	}
	else
	{
		struct FileNode* temp = file->first;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = (struct FileNode*) malloc(sizeof(struct FileNode));
		temp = temp->next;
		temp->name = name;
		temp->next = NULL;
		temp->size = size;
	}
}

void AddFile(struct SimilarFile* head, char* name, int size)
{
	struct SimilarFile* temp = head;
	while(temp->next != NULL)
	{
		temp = temp->next;
		if(CheckForSimillar(name, temp->name))
		{
			temp->n++;
			AddDir(temp, name, size);
			return;
		}
	}
	temp->next = (struct SimilarFile*) malloc(sizeof(struct SimilarFile));
	temp = temp->next;
	temp->name = name;
	temp->n = 1;
	temp->next = NULL;
	temp->first = NULL;
	AddDir(temp, name, size);
	return;
}

struct SimilarFile head;

void FindSimilarDirs(DIR *dr, int currDir, char* dirStr)
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
				FindSimilarDirs(dr1, Fd, newDir);
			
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
				int tempLen = strlen(dirStr);
				if(dirStr[tempLen-1] != '/')
					tempLen++;
				char *newDir = (char*)malloc(tempLen + strlen(d->d_name) + 1);
				strcpy(newDir, dirStr);
				newDir[tempLen] = 0;
				newDir[tempLen - 1] = '/';
				strcat(newDir, d->d_name);
				
				AddFile(&head, newDir, st1.st_size);
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

void writeAllSimilarFiles()
{
	struct SimilarFile* thead = &head;
	
	while(thead->next != NULL)
	{
		thead = thead->next;
		if(thead->n>1)
		{
			fprintf(stdout, "Files simillar to %s\n", thead->name);
			struct FileNode* temp = thead->first;
			while(temp != NULL)
			{
				fprintf(stdout, "%s  (%d)\n", temp->name, temp->size);
				temp = temp->next;
			}
			fprintf(stdout, "-------------------\n");
		}
	}
	return;
}

void main(int argc, char *argv[])
{
	if(argc != 4)
	{
		fprintf(stderr, "1st parametr - directory, 2nd - min size(byte), 3rd - max size(byte)\n");
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
	
	head.next = NULL;
	
	FindSimilarDirs(dr, F, argv[1]);

	writeAllSimilarFiles();
	
	if(closedir(dr) == -1)
	{
		perror("closedir");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
