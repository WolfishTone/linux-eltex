.PHONY: task1 task2-serial task2-multithreaded task2-mutex task2-spinlock task2-mapreduce task3-store

all: task1 task2-serial task2-multithreaded task2-mutex task2-spinlock task2-mapreduce task3-store

task1: task1.c
	gcc -Wall -Wextra $^ -o $@ -g

task2-serial: task2-serial.c
	gcc -Wall -Wextra $^ -o $@ -g

task2-multithreaded: task2-multithreaded.c
	gcc -Wall -Wextra $^ -o $@ -g
	
task2-mutex: task2-mutex.c
	gcc -Wall -Wextra $^ -o $@ -g
	
task2-spinlock: task2-spinlock.c
	gcc -Wall -Wextra $^ -o $@ -g
	
task2-mapreduce: task2-mapreduce.c
	gcc -Wall -Wextra $^ -o $@ -g
	
task3-store: task3-store.c
	gcc -Wall -Wextra $^ -o $@ -g
