.PHONY: all task1

SRCS=$(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: task_1

run:
	LD_LIBRARY_PATH=./lib:LD_LIBRARY_PATH ./calc

task_1: obj/main.o
	gcc -Wall -Wextra $< -o $@

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -fPIC
	
	
format:
	clang-format -style=GNU $(SRCS) -i
