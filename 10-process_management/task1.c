#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	printf("Parent pid = %d ppid = %d\n", getpid(), getppid());
	pid_t child = fork();
	int status;
	if(child)
	{
		wait(&status);
		printf("child status = %d\n", WEXITSTATUS(status));
	}
	else
	{
		printf("Child pid = %d ppid = %d\n", getpid(), getppid());
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_SUCCESS);
}
