#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define ITER_PER_THREAD 30000000
#define THREAD_NUM 6

long a = 0;
pthread_rwlock_t  rwlock = PTHREAD_RWLOCK_INITIALIZER;

int sum(void* in)
{
	pthread_rwlock_trywrlock(&rwlock);
	long local_a = a;
	pthread_rwlock_unlock(&rwlock);
	
	for(int i= 0; i< ITER_PER_THREAD; i++)
		local_a++;
		
	pthread_rwlock_trywrlock(&rwlock);
	a+= local_a;
	pthread_rwlock_unlock(&rwlock);
	pthread_exit(0);
}

int main()
{
	pthread_t threads[5];
	int *s;
	
	//~ pthread_mutex_init(&mut,0);
	
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
