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
#include <sys/un.h>

#define BUFLEN 255
#define SOCKET_NAME_SERVER "/tmp/test1_local_stream_server.socket\0"

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_un servAddr;

    if(argc < 2) { 
		printf(" ВВЕСТИ сообщение\n");
		exit(1) ;
	}
	
    if((sock = socket( AF_LOCAL, SOCK_STREAM, 0 ) ) < 0) {
		perror("He могу получить socket\n");
		exit(1);
	}

    memset(&servAddr, '\0', sizeof(servAddr));  
    servAddr.sun_family = AF_LOCAL;
    memcpy(servAddr.sun_path, SOCKET_NAME_SERVER, sizeof(SOCKET_NAME_SERVER));

    int l = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);
	
	if( connect( sock, (struct sockaddr *)&servAddr, l) < 0 ){
		perror("Клиент не может соединиться.\n");
		exit(1);
	}
	
	printf ("CLIENT: Готов к пересылке\n") ;
	
	for (int i = 1; i <= 10; ++i) {
        int msgLength;		
		if((msgLength = send ( sock, argv[1], strlen(argv[1]), 0 )) < 0 ) {
			perror( "Проблемы с пересылкой.\n" ); 
			exit(1);
		}
        printf ("CLIENT: отправка сообщения длиной %d: %s\n", msgLength, argv[1]) ;

		sleep(1);
		char buf[BUFLEN];

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
