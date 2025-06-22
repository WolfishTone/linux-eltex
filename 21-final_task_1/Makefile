SRCS= $(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, $(SRCS))

.PHONY: all $$BINS

all: bin/RAW_echo_client bin/RAW_echo_server

bin/RAW_echo_client: src/RAW_echo_client.c
	gcc -Wall -Wextra $^ -o $@ 

bin/RAW_echo_server: obj/RAW_echo_server.o obj/splaytree.o
	gcc -Wall -Wextra $^ -o $@ -I include -g

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -g -I include
