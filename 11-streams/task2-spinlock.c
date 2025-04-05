#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define ITER_PER_THREAD 30000000
#define THREAD_NUM 6

long a = 0;
pthread_spinlock_t spin;

int sum(void* in)
{
	for(int i= 0; i< ITER_PER_THREAD; i++)
	{
		pthread_spin_lock(&spin);
		a++;
		pthread_spin_unlock(&spin);
	}
	pthread_exit(0);
}

int main()
{
	pthread_t threads[5];
	int *s;
	
	pthread_spin_init(&spin,PTHREAD_PROCESS_PRIVATE);
	
	for(int i= 0; i< THREAD_NUM; i++) {
		if (pthread_create(&threads[i], NULL, (void *) sum, NULL) != 0) {
			perror("Не удалось создать поток. \n");
			exit(1);
		}
	}
	
	for(int i= 0; i< THREAD_NUM; i++)
		pthread_join(threads[i], (void**) &s);
	
	printf("sum = %ld\n", a);
	exit(0);
}
