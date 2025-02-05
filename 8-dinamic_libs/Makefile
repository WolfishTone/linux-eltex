.PHONY: all run calc format

SRCS=$(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: calc

run:
	LD_LIBRARY_PATH=./lib:LD_LIBRARY_PATH ./calc

calc: obj/main.o lib/libcalc.so
	gcc -Wall -Wextra $< -o $@ -O2 -L./lib -lcalc

lib/libcalc.so: $(OBJS)
	gcc -shared $^ -o $@

obj/main.o : src/main.c
	gcc -Wall -Wextra -c $^ -o $@ -I ./include

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -fPIC
	
	
format:
	clang-format -style=GNU $(SRCS) -i
