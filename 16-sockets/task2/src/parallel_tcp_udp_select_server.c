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
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

#define BUFLEN 255


int main(int argc, char *argv[]){
	if(argc< 3)
	{
		printf("ВВЕДИ udp_port tcp_port\n");
		exit(1);
	}

	fd_set read_fds, tcp_active_clients;
	int tcp_sock, udp_sock;
	struct sockaddr_in tcp_sock_addr, udp_sock_addr;
	

	if(( tcp_sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть tcp_socket.\n"); 
		exit(1);
	}
	int flags = fcntl(tcp_sock, F_GETFL, 0); // получаем флаги сокета
	if(fcntl(tcp_sock, F_SETFL, flags | O_NONBLOCK) != 0) // делаем сокет неблокирующим
	{
		perror("Настройка неблокируещего сокета для tcp неудачна.\n"); 
		exit(1);
	}
	memset(&tcp_sock_addr, '\0', sizeof(tcp_sock_addr));
	tcp_sock_addr.sin_family = AF_INET;
	tcp_sock_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	tcp_sock_addr.sin_port = htons((in_port_t)atoi(argv[2]));
	if ( bind( tcp_sock, (struct sockaddr *)&tcp_sock_addr, sizeof(tcp_sock_addr) ) ) {
		perror("Связывание tcp_socket неудачно.\n"); 
		exit(1);
	}
	int length = sizeof( tcp_sock_addr ) ;
	if ( getsockname( tcp_sock, (struct sockaddr *)&tcp_sock_addr, (socklen_t *)&length ) ){
		perror("Вызов getsockname неудачен.\n");
		exit(1);
	}
	printf("SERVER: tcp_socket готов: %s %d\n", inet_ntoa(tcp_sock_addr.sin_addr), ntohs(tcp_sock_addr.sin_port));


	if(( udp_sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть udp_socket.\n"); 
		exit(1);
	}
	flags = fcntl(udp_sock, F_GETFL, 0); // получаем флаги сокета
	if(fcntl(udp_sock, F_SETFL, flags | O_NONBLOCK) != 0) // делаем сокет неблокирующим
	{
		perror("Настройка неблокируещего сокета для tcp неудачна.\n"); 
		exit(1);
	}
	memset(&udp_sock_addr, '\0', sizeof(udp_sock_addr));
	udp_sock_addr.sin_family = AF_INET;
	udp_sock_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	udp_sock_addr.sin_port = htons((in_port_t)atoi(argv[1]));
	if ( bind( udp_sock, (struct sockaddr *)&udp_sock_addr, sizeof(udp_sock_addr) ) ) {
		perror("Связывание udp_socket неудачно.\n"); 
		exit(1);
	}
	length = sizeof( udp_sock_addr ) ;
	if ( getsockname( udp_sock, (struct sockaddr *)&udp_sock_addr, (socklen_t *)&length ) ){
		perror("Вызов getsockname неудачен.\n");
		exit(1);
	}
	printf("SERVER: udp_socket готов: %s %d\n", inet_ntoa(udp_sock_addr.sin_addr), ntohs(udp_sock_addr.sin_port));

	int fds_num_limit = 1024;	

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	FD_ZERO(&tcp_active_clients);
	
	listen(tcp_sock, 5);
	for( ; ; ) {
		memcpy(&read_fds, &tcp_active_clients, sizeof(read_fds));
		FD_SET(tcp_sock, &read_fds);
    	FD_SET(udp_sock, &read_fds);

		int select_fds_num = select(fds_num_limit, &read_fds, (fd_set *)0, (fd_set *)0, &tv);
		if(select_fds_num<0) {
			perror("Вызов select неудачен.\n");
			exit(-1);
		}
		if(select_fds_num == 0) {
			//printf("нет активных сокетов");
			continue;
		}
		
		char buf[BUFLEN];
		char send_buf[BUFLEN];
		int msgLength;
		int sockClient;

		if(FD_ISSET(tcp_sock, &read_fds)) // подключение tcp клиентов
		{
			printf("SERVER: обработка запроса на подключение tcp клиента\n");
			if ( ( sockClient = accept( tcp_sock, 0, 0 ) ) > 0 ) {
				FD_SET(sockClient, &tcp_active_clients);
				printf("SERVER: клиент %d был успешно подключен\n", sockClient);
			}
		}
			
		for (int fd = 3 ; fd < 10 ; fd++ ){ // проход по всевозможным сокетам и обработка активных
			if(FD_ISSET(fd, &tcp_active_clients)) 
			{
				printf("SERVER: обработка запроса tcp клиента %d\n", fd);
				if((msgLength = recv ( fd, buf, BUFLEN, 0 )) < 0 ) {
					perror( "Проблемы с пересылкой.\n" ); 
					exit(1);
				}
				if(msgLength == 0)
				{
					close(fd);
					FD_CLR(fd, &tcp_active_clients);
					printf("клиент %d отключился\n", fd);
					continue;
				}

				if(strcmp(buf, "get time\0"))
					continue;
			
				// get current time
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				sprintf(send_buf, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

				if( send ( fd, send_buf, strlen(send_buf), 0 ) < 0 ) {
					perror( "Проблемы с пересылкой.\n" ); 
					exit(1);
				}
			}
		}

		if(FD_ISSET(udp_sock, &read_fds))
		{
			printf("SERVER: обработка запроса udp клиента\n");
			struct sockaddr_in clientAddr;
			memset(&clientAddr, '\0', sizeof(clientAddr));

			length = sizeof(clientAddr);
			if ((msgLength = recvfrom(udp_sock, &buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
				perror("Плохой socket клиента.");
				exit(1);
			}

			// get current time
			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			sprintf(send_buf, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

			if (sendto(udp_sock, &send_buf, sizeof(send_buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
				perror("Проблемы с sendto . \n");
				exit(1);
			}
		}
	}
}

