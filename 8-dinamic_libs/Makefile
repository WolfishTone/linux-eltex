.PHONY: all abonent password_validate

SRCS=$(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

all: calc

run:
	

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
