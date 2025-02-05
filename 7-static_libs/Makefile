.PHONY: all calc format

SRCS=$(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: calc


calc: lib/libcalc.a
	gcc -Wall -Wextra $^ -o $@ -O2 -L./lib -lcalc

lib/libcalc.a: $(OBJS)
	ar rc $@ $^

obj/main.o : src/main.c
	gcc -Wall -Wextra -c $^ -o $@ -I ./include

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@
	
	
format:
	clang-format -style=GNU $(SRCS) -i
