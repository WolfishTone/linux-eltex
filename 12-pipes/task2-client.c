#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
	char pipe_name[] = "my_pipe";
	char buf[4];
	
	int pipe = open(pipe_name, O_RDONLY);
	if(pipe < 0)
	{
		perror("ошибка открытия именованного канала\n");
		exit(1);
	}
	
	if(read(pipe, buf, 4) != 4)
	{
		perror("ошибка чтения.\n");
		exit(1);
	}
	
	printf("message: %s\n", buf);
	
	close(pipe);
	exit(0);
}
