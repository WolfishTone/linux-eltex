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

int main(){

    unlink(SOCKET_NAME_SERVER);
	int sockMain, sockClient;
	struct sockaddr_un servAddr;
	
	if(( sockMain = socket( AF_LOCAL, SOCK_STREAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть главный socket.\n"); 
		exit(1);
	}
	
	memset(&servAddr, '\0', sizeof(servAddr));  
    servAddr.sun_family = AF_LOCAL;
    memcpy(servAddr.sun_path, SOCKET_NAME_SERVER, sizeof(SOCKET_NAME_SERVER));

    int l = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);
    if (bind(sockMain, (struct sockaddr *)&servAddr, l)) {
        perror("Связывание клиента неудачно.");
        exit(1);
    }
	
	listen(sockMain, 2) ;
	
	for( ; ; ) {
        struct sockaddr_un clientAddr;
        socklen_t lenght = sizeof(clientAddr);
		if ( ( sockClient = accept( sockMain, (struct sockaddr *)&clientAddr, &lenght ) ) < 0 ) {
			perror("Неверный socket для клиента. \n");
			exit(1);
		}

        char buf[BUFLEN];
        memset(buf, '\0', BUFLEN);
        
        int msgLength;
        
        while( ( msgLength = recv( sockClient, buf, BUFLEN, 0 ) ) > 0 ){
            printf ("CLIENT: получение сообщения длиной %d: %s\n", msgLength, buf) ;
            if((msgLength = send ( sockClient, buf, strlen(buf), 0 )) < 0 ) {
				perror( "Проблемы с пересылкой.\n" ); 
				exit(1);
			}
            printf ("CLIENT: отправка сообщения длиной %d: %s\n\n", msgLength, buf) ;
			memset(buf, '\0', BUFLEN);

        }
		close(sockClient);
	}
}

