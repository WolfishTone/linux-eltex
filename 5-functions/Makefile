.PHONY: all abonent password_validate

SRCS=$(wildcard *.c) 
BINS= $(patsubst %.c, %, $(SRCS))

all: $(BINS)

#~ abonent: abonent.c
#~ 	gcc -Wall $^ -o $@ -O2

#~  -fno-stack-protector флаг, отключающий "канарейку"
#~  -no-pie флаг отключающий варьирование адресов
#~  -Wno-deprecated-declarations подключение устаревших объявлений. среди них gets() 
password_validate: password_validate.c
	gcc -Wall -g -fno-stack-protector -no-pie -Wno-deprecated-declarations $^ -o $@ -z execstack
	
format:
	clang-format -style=GNU $(SRCS) -i
	
#~ 00 00 00 00 00 40 11 d2
