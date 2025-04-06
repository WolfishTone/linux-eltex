.PHONY: task1 task2-client task2-server mybash task3-mybash

all : task1 task2-client task2-server task3-mybash

task1: task1.c
	gcc -Wall -Wextra $^ -o $@ -g
	
task2-client: task2-client.c
	gcc -Wall -Wextra $^ -o $@ -g

task2-server: task2-server.c
	gcc -Wall -Wextra $^ -o $@ -g


task3-mybash: mybash/obj/main.o mybash/obj/pipeline.o mybash/obj/command.o
	gcc -Wall -Wextra $^ -o $@ -g
	
mybash/obj/%.o : mybash/src/%.c
	gcc -Wall -Wextra -c $< -o $@ -I mybash -g
