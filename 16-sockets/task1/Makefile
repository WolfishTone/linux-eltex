SRCS= $(wildcard src/*.c)
BINS= $(patsubst src/%.c, bin/%, $(SRCS))

.PHONY: all $$BINS

all: $(BINS)

bin/% : src/%.c
	gcc -Wall -Wextra  $^ -o $@ -g