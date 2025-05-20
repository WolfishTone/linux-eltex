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

  if (argc < 3) {
    printf(" ВВЕСТИ udpclient имя_хоста порт \n");
    exit(1);
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

  int length = 0, msgLength = 0;
  printf("CLIENT: Готов к пересылке.\n");
  char buf[] = "get time\0";
  char recv_buf[BUFLEN];

  for (int i = 1; i < 10; ++i) { //
    if (sendto(sock, buf, strlen(buf)+1, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }
    if ((msgLength = recvfrom(sock, recv_buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
      perror("Плохой socket клиента.");
      exit(1);
    }
    printf("CLIENT: получено текущее время: %s\n", recv_buf);
    sleep(i);
  }

  printf("CLIENT: Пересылка завершена. Счастливо оставаться. \n");
  close(sock);
  exit(0);
}
