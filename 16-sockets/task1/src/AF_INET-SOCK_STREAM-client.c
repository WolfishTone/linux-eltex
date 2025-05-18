#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFLEN 255

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in servAddr;
	struct hostent *hp;
	
	if(argc < 4) { 
		printf(" ВВЕСТИ udpclient имя_хоста порт сообщение\n");
		exit(1) ;
	}
	if((sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0) {
		perror("He могу получитьsocket\n");
		exit(1);
	}
	
	memset(&servAddr, '\0', sizeof(servAddr));
	servAddr.sin_family = AF_INET;

	hp = gethostbyname(argv[1]);
	memcpy(&servAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
	servAddr.sin_port = htons(atoi(argv[2]));
	
	if( connect( sock, (struct sockaddr *)&servAddr, sizeof(servAddr) ) < 0 ){
		perror("Клиент не может соединиться.\n");
		exit(1);
	}
	
	printf ("CLIENT: Готов к пересылке\n") ;
	
	for (int i = 1; i <= 10; ++i) {
		printf ("CLIENT: отправка сообщения длиной %ld: %s\n", strlen(argv[3]), argv[3]) ;
		
		if( send ( sock, argv[3], strlen(argv[3]), 0 ) < 0 ) {
			perror( "Проблемы с пересылкой.\n" ); 
			exit(1);
		}

		sleep(1);
		char buf[BUFLEN];
		int msgLength;

		if((msgLength = recv ( sock, buf, BUFLEN, 0 )) < 0 ) {
			perror( "Проблемы с пересылкой.\n" ); 
			exit(1);
		}

		printf ("CLIENT: получение сообщения длиной %d: %s\n", msgLength, buf) ;
	}
	
	printf("CLIENT: Пересылка завершена. Счастливо оставаться.\n");
	close(sock); 
	exit(0);
}
