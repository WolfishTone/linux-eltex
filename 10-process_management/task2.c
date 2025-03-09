#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t child1, child2 = 1;
	printf("Parent: pid = %d ppid = %d\n", getpid(), getppid());
	
	child1 = fork();
	if(child1>0 && child2>0)
	{
		child2 = fork();
		if(child2)
		{
			int status1, status2;
			wait(&status1);
			wait(&status2);
			printf("Parent: status1 = %d\n", WEXITSTATUS(status1));
			printf("Parent: status2 = %d\n", WEXITSTATUS(status2));
		}
	}
	if(child1 == 0)
	{
		printf("Child1: pid = %d ppid = %d\n", getpid(), getppid());
		pid_t child3, child4 = 1;
		child3 = fork();
		if(child3>0 && child4>0)
		{
			child4 = fork();
			if(child4)
			{
				int status3, status4;
				wait(&status3);
				wait(&status4);
				printf("Child1: status1 = %d\n", WEXITSTATUS(status3));
				printf("Child1: status2 = %d\n", WEXITSTATUS(status4));
			}
		}
		if(child3 == 0)
		{
			printf("Child3: pid = %d ppid = %d\n", getpid(), getppid());
			exit(3);
		}
		if(child4 == 0)
		{
			printf("Child4: pid = %d ppid = %d\n", getpid(), getppid());
			exit(4);
		}
		
		exit(1);
	}
	if(child2 == 0)
	{
		printf("Child2: pid = %d ppid = %d\n", getpid(), getppid());
		pid_t child5 = fork();
		int status5;
		if(child5 > 0)
		{
			wait(&status5);
			printf("Child2: status5 = %d\n", WEXITSTATUS(status5));
		}
		else
		{
			printf("Child5: pid = %d ppid = %d\n", getpid(), getppid());
			exit(5);
		}
		exit(2);
	}
	exit(EXIT_SUCCESS);
}

