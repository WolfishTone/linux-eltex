#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


int main()
{
	char pipe_name[] = "my_pipe";
	int a;
	if (mkfifo(pipe_name, S_IRWXU) != 0 && errno!= 17)    // если именованный канал существует, то 
	{												      // mkfifo вернет ошибку. чтобы не пересоздавать канал, 
		perror("ошибка создания именованного канала\n");  // используем уже существуюший.
		exit(1);
	}
	
	int pipe = open(pipe_name, O_WRONLY);
	if(pipe < 0)
	{
		perror("ошибка открытия именованного канала\n");
		exit(1);
	}
	
	if(write(pipe, "Hi!", 4) != 4)
	{
		perror("ошибка записи.\n");
		exit(1);
	}
	
	unlink(pipe_name); // удаление именованного канала
	close(pipe);
	exit(0);
}
