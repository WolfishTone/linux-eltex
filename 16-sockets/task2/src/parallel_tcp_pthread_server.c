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
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#define BUFLEN 255


void threadclient (int *socket)
{
	int local_socket = *socket;

	printf("\nсоздан поток с id: %ld\n\n", pthread_self());

	char buf[BUFLEN];
	memset(buf, '\0', BUFLEN);

	int msgLength;
	while( ( msgLength = recv( local_socket, buf, BUFLEN, 0 ) ) > 0 ){
		printf( "SERVER: сообщение от клиента %d длиной %d: %s\n",
			local_socket, msgLength, buf);
        if(strcmp(buf, "get time\0"))
			continue;

        // get current time
        char buf[BUFLEN];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(buf, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

        if( send (local_socket, buf, strlen(buf), 0 ) < 0 ) {
			perror( "Проблемы с пересылкой.\n" );
			exit(1);
		}
		memset(buf, '\0', BUFLEN);
	}

	close( local_socket ) ;
	printf( "поток %ld закрыт\n", pthread_self());
	pthread_exit(0);
}

void finalize( int sig ){
	printf("SERVER: Пересылка завершена. Счастливо оставаться.\n");
	exit(0);
}

int main(){
	signal( SIGINT, finalize );

	pthread_t thread;
	int sockMain, sockClient, length;
	struct sockaddr_in servAddr;

	if(( sockMain = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть главный socket.\n");
		exit(1);
	}

	memset(&servAddr, '\0', sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servAddr.sin_port = 0;

	if ( bind( sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr) ) ) {
		perror("Связывание сервера неудачно.\n");
		exit(1);
	}

	length = sizeof( servAddr ) ;

	if ( getsockname( sockMain, (struct sockaddr *)&servAddr, (socklen_t *)&length ) ){
		perror("Вызов getsockname неудачен.\n");
		exit(1);
	}

	printf("SERVER: IP адрес - %s номер порта - % d\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

	listen(sockMain, 2) ;

	for( ; ; ) {
		if ( ( sockClient = accept( sockMain, 0, 0 ) ) < 0 ) {
			perror("Неверный socket для клиента. \n");
			exit(1);
		}
		printf("sockClient = %d\n", sockClient);
		if (pthread_create(&thread, NULL, (void *) threadclient, (int *) &sockClient) <0){
			perror("Не удалось создать поток. \n");
			exit(1);
		}
		if (pthread_detach(thread) != 0) {
			perror("pthread_detach() error");
			exit(1);
		}
	}
}

