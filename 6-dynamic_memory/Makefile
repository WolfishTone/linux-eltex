.PHONY: all abonent password_validate

SRCS=$(wildcard src/*.c) 
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: dynamic_mem_abonent

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -I .

dynamic_mem_abonent: $(OBJS)
	gcc -Wall -Wextra $^ -o $@ -O2
	
format:
	clang-format -style=GNU $(SRCS) -i
