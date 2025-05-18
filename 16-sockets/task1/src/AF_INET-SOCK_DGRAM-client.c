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

int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in servAddr, clientAddr;
  struct hostent *hp;
  
  if(argc < 4) { 
    printf(" ВВЕСТИ udpclient имя_хоста порт сообщение\n");
    exit(1) ;
  }
  
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("He могу получить socket\n");
    exit(1);
  }
  
  memset(&servAddr, '\0', sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  
  hp = gethostbyname(argv[1]);
  memcpy(&servAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
  servAddr.sin_port = htons(atoi(argv[2]));
  
  memset(&clientAddr, '\0', sizeof(clientAddr));
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddr.sin_port = 0;
  
  if (bind(sock, (struct sockaddr *)&clientAddr, sizeof(clientAddr))) {
    perror("Клиент не получил порт.");
    exit(1);
  }

  if( connect( sock, (struct sockaddr *)&servAddr, sizeof(servAddr) ) < 0 ){
		perror("Клиент не может соединиться.\n");
		exit(1);
	}
  
  int length = 0, msgLength = 0;
  printf("CLIENT: Готов к пересылке.\n");
  
  for (int i = 0; i < 10; ++i) {
    printf ("CLIENT: отправка сообщения длиной %ld: %s\n", strlen(argv[3]), argv[3]) ;
    if (sendto(sock, argv[3], sizeof(argv[3]), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }

    sleep(1);

    char buf[BUFLEN];

    if ((msgLength = recvfrom(sock, &buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
      perror("Плохой socket клиента.");
      exit(1);
    }
    printf ("CLIENT: получение сообщения длиной %d: %s\n", msgLength, buf) ;
  }

  printf("CLIENT: Пересылка завершена. Счастливо оставаться. \n");
  close(sock);
  exit(0);
}
