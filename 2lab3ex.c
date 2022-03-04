#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

void main(int argc, char *argv[ ])
{
	if(argc != 2)
	{
		fprintf(stderr, "Ошибка: Программа требует одного аргумента.\n");
		exit(EXIT_FAILURE);
	}
	
	FILE *F1 = fopen(argv[1], "w");
	if (F1 == NULL){
		//fprintf(stderr, "Ошибка создания (открытия) файла. Ошибка номер %d\n", errno);
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	char input = fgetc(stdin);
	while(input != '')
	{
		if(fputc(input, F1) == EOF)
		{
			fprintf(stderr, "Ошибка записи символа.");
			exit(EXIT_FAILURE);
		}
		input = getchar();
	}
	if (fclose(F1) == EOF)
	{
		//fprintf(stderr, "Ошибка закрытия файла. Ошибка номер %d\n", errno);
		perror("fclose");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
