.PHONY: task1-POSIX task1-SYSTEM-V task2 task2/client task2/server

task1-POSIX: task1-POSIX/client task1-POSIX/server

task1-POSIX/client: task1-POSIX/client.c
	gcc -Wall -Wextra $^ -o $@ -g -lrt -pthread
task1-POSIX/server: task1-POSIX/server.c
	gcc -Wall -Wextra $^ -o $@ -g -lrt -pthread


task1-SYSTEM-V: task1-SYSTEM-V/client task1-SYSTEM-V/server

task1-SYSTEM-V/client: task1-SYSTEM-V/client.c
	gcc -Wall -Wextra $^ -o $@ -g -lrt
task1-SYSTEM-V/server: task1-SYSTEM-V/server.c
	gcc -Wall -Wextra $^ -o $@ -g -lrt

task2: task2/server task2/client

task2/client: task2/obj/client.o task2/obj/Communication.o task2/obj/UsersList.o task2/obj/UI.o task2/obj/Msg_history.o
	gcc -Wall -Wextra $^ -o $@ -lncurses 

task2/server: task2/obj/server.o task2/obj/Communication.o task2/obj/UsersList.o
	gcc -Wall -Wextra $^ -o $@

task2/obj/%.o : task2/src/%.c
	gcc -Wall -Wextra -MMD -c $^ -o $@ -I ./task2/include -g

clear:
	rm obj/*

kill:
	killall client server; \
	Sems=$$(ipcs -s | grep -o -e '[ ][1-9][0-9][0-9][0-9][0-9][0-9]' | sed -e 's/^/-s/'); \
	if [ -n "$$Sems" ]; then \
		ipcrm $$Sems; \
	else \
		echo "Нет семафоров для удаления"; \
	fi; \
	ipcrm -M 0x0f06004e -M 0x1006004f -M 0x1106008f