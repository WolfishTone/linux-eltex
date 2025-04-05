#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void* show_id(void* in)
{
	int *i = (int *)in;
	printf("pid_ind = %d\n", *i);
	pthread_exit(0);
}

int main()
{
	pthread_t thread;
	int i_m[5];
	
	for(int i= 0; i< 5; i++)
	{
		i_m[i]=i;
		if (pthread_create(&thread, NULL, (void *) show_id, (void*) &i_m[i]) != 0) {
			perror("Не удалось создать поток. \n");
			exit(1);
		}
		if (pthread_detach(thread) != 0) {
			perror("Не удалось открепить поток\n");
			exit(1);
		}
	}
	
	/* при использовании открепленных потоков, основной поток может завершиться до конца 
	 * работы дочерних. это приводит к тому, что такие потоки не могут вывести, переданное
	 * им число. чтобы это решить было добавляено ожидание в конце. при использовании join
	 * таких проблем не возникает.*/
	
	sleep(1);	
	exit(0);
}
