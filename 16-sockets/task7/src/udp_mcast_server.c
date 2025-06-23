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



int main()
{
	int sock;
	if(( sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("can't get socket ");
		exit(1);
	}

	char client_ip[4] = {224, 0, 0, 2};

	struct sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	memcpy(&(clientAddr.sin_addr), client_ip, sizeof(char)*4); // 224.0.0.2
	clientAddr.sin_port = htons(7777);

	char buf[BUFLEN];
	int msgLength;

	for(int i = 0; i < 100000; i++)
	{
		sprintf(buf, "hello net %d", i);
		if ((msgLength = sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr))) < 0) {
		  perror("sendto() problem ");
		  exit(1);
		}

		printf ("SERVER: sent msg '%s' [%d]\n", buf, msgLength);
		sleep(2);
	}

}
