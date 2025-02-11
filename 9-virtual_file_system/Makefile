.PHONY: all task1

SRCS=$(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: task_1 sunset-commander

task_1: obj/task-1.o
	gcc -Wall -Wextra $< -o $@
sunset-commander: obj/sunset-commander.o
	gcc -Wall -Wextra $< -o $@ -lncurses

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -fPIC

format:
	clang-format -style=GNU $(SRCS) -i
