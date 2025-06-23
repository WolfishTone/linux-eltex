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

	struct sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = 4294967295; // 255.255.255.255
	clientAddr.sin_port = htons(7777);


	// bind to get endpoint.
	if ( bind( sock, (struct sockaddr *)&clientAddr, sizeof(clientAddr) ) ) {
		perror("bind() problem ");
		exit(1);
	}

	char buf[BUFLEN];
	int msgLength;

	while(1)
	{
		int length = sizeof (clientAddr);
		if ((msgLength = recvfrom(sock, &buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
		  perror("recvfrom() problem ");
		  exit(1);
		}

		printf ("CLIENT: got msg '%s' [%d]\n", buf, msgLength);
	}

}
