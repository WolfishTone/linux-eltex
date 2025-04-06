#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	int mypipe[2];
	
	if (pipe (mypipe))
    {
      printf("Проблема при создании канала.\n");
      exit(1);
    }
    
    pid = fork ();
	if (pid == (pid_t) 0)
    {
		printf("child\n");
		char buf [4];
		close (mypipe[1]); // закрываем дискриптор на запись
		if(read(mypipe[0], buf, 4) != 4)
		{
			perror("ошибка чтения.\n");
			exit(1);
		}
		printf("message: %s\n", buf);
		close (mypipe[0]);
    }
	else
    {
		close (mypipe[0]); // закрываем дискриптор на чтение
		if(write(mypipe[1], "Hi!", 4) != 4)
		{
			perror("ошибка записи.\n");
			exit(1);
		}
		close (mypipe[1]);
    }
    
	exit(0);
}
