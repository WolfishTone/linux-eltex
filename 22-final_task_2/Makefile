

all: taxi_station

taxi_station: obj/main.o obj/CLI.o obj/my_IO.o obj/driver.o
	gcc -Wall -Wextra $^ -o $@ -I .

obj/%.o : src/%.c
	gcc -Wall -Wextra -c $^ -o $@ -MMD -g -I .