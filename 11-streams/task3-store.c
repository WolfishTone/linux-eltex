#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define CUSTOMER_NUM 3
#define STORE_EMPLOYEE_NUM 1

#define THREAD_NUM CUSTOMER_NUM+STORE_EMPLOYEE_NUM

#define STORE_SIZE 5

int store[STORE_SIZE];

pthread_rwlock_t  rwlock = PTHREAD_RWLOCK_INITIALIZER;


void print_store_condition()
{
	printf("Состояние магазина:\n");
	for(int i= 0; i< STORE_SIZE; i++)
		printf("комната %5d: %d\n", i, store[i]);
}


void * customer(void* in)
{
	int *num = (int *)in;
	int need = 10000 + (rand()%2000);
	
	while(1)
	{
		sleep(1);
		printf("\nВ магазин входит покупатель %d. Его потребность: %d\n", *num, need);
		pthread_rwlock_trywrlock(&rwlock);
		//~ print_store_condition();
		while(1)
		{
			int room_num = rand()%STORE_SIZE;
			if(store[room_num])
			{
				need-= store[room_num];
				store[room_num] = need>=0?0: store[room_num]+need;
				break;
			}
		}
		pthread_rwlock_unlock(&rwlock);
		printf("Покупатель %d выходит из магазина.", *num);
		print_store_condition();
		if(need < 0)
		{
			printf(" Его потребности удволетворены.n");
			pthread_exit(0);
		}
		printf("\n");
	}
}

void * store_employee(void* in)
{
	int *num = (int *)in;
	int loc_store[STORE_SIZE]; /* копия массива с кол-вом товара с предыдущего шага
			  * используется для сравнения с количеством товаров на предыдущем шаге*/		   
	for(int i= 0; i< STORE_SIZE;i++)
		loc_store[i]= store[i];
	loc_store[0] += 10;
	while(1)
	{
		sleep(2);
		printf("\nВ магазин входит работник магазина %d \n", *num);
		pthread_rwlock_trywrlock(&rwlock);
		
		int diff_count = 0;
		for(int i = 0; i< STORE_SIZE;i++)
			if(loc_store[i] == store[i])
				diff_count++;
				
		if(diff_count == STORE_SIZE)
		{
			printf("Работник магазина %d выходит из магазина. Все клиенты удволетворены.\n", *num);
			pthread_exit(0);
		}
		
		for(int i = 0; i< STORE_SIZE;i++)
			store[i]+=500;
		
		pthread_rwlock_unlock(&rwlock);
		
		printf("Работник магазина %d выходит из магазина.\n", *num);
		print_store_condition();
		
		for(int i= 0; i< STORE_SIZE;i++)
			loc_store[i]= store[i];
	}
}


int main()
{
	pthread_t threads[THREAD_NUM];
	int *s;
	
	for(int i= 0; i< STORE_SIZE;i++)
		store[i]= 500;
	
	int customer_nums[CUSTOMER_NUM];
	for(int i= 0; i< CUSTOMER_NUM; i++) {
		customer_nums[i] = i;
		if (pthread_create(&threads[i], NULL, (void *) customer, (void*) &customer_nums[i]) != 0) {
			perror("Не удалось создать покупателя. \n");
			exit(1);
		}
	}
	
	int employee_num[STORE_EMPLOYEE_NUM];
	for(int i= 0; i< STORE_EMPLOYEE_NUM; i++) {
		employee_num[i] = i;
		if (pthread_create(&threads[i], NULL, (void *) store_employee, (void*) &employee_num[i]) != 0) {
			perror("Не удалось создать работника магазина. \n");
			exit(1);
		}
	}
	
	for(int i= 0; i< THREAD_NUM; i++)
		pthread_join(threads[i], (void**) &s);
	
	exit(0);
}
