.PHONY: all clean build

SRCS=$(wildcard *.c) 
BINS= $(patsubst %.c, %, $(SRCS))

all: $(BINS)

%: %.c
	gcc -Wall -Wextra -Werror $^ -o $@ -O2
	
format:
	clang-format -style=GNU $(SRCS) -i
