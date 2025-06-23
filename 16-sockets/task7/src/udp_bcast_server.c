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
	int flag = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

	struct sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = 4294967295; // 255.255.255.255
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
