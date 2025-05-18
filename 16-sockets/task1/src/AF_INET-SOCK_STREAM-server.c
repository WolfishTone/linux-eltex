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

int main(){

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

        char buf[BUFLEN];
        memset(buf, '\0', BUFLEN);
        
        int msgLength;
        
        while( ( msgLength = recv( sockClient, buf, BUFLEN, 0 ) ) > 0 ){
            printf( "SERVER: Socket для клиента - %d\n", sockClient);
            printf( "SERVER: сообщение длиной %d: %s\n", msgLength, buf);
            if( send ( sockClient, buf, strlen(buf), 0 ) < 0 ) {
				perror( "Проблемы с пересылкой.\n" ); 
				exit(1);
			}
			memset(buf, '\0', BUFLEN);

        }
		close(sockClient);
	}
}

