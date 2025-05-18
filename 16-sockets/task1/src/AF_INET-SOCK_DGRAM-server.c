#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFLEN 255

int main() {
  int sockMain, length, msgLength;
  struct sockaddr_in servAddr, clientAddr;
  
  if ((sockMain = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Сервер не может открыть socket для UDP.");
    exit(1);
  }
  
  memset(&servAddr, '\0', sizeof(servAddr));
  memset(&clientAddr, '\0', sizeof(clientAddr));
  
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = 0; 
    
  if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
    perror("Связывание сервера неудачно.");
    exit(1);
  }
  
  length = sizeof(servAddr);
  
  if (getsockname(sockMain, (struct sockaddr *)&servAddr, (socklen_t *)&length)) {
    perror("Вызов getsockname неудачен.");
    exit(1);
  }
  
  printf("SERVER: IP адрес - %s номер порта - % d\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));
  for (;;) {
    length = sizeof(clientAddr);
    
    char buf[BUFLEN];
    if ((msgLength = recvfrom(sockMain, &buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
      perror("Плохой socket клиента.");
      exit(1);
    }
    printf("SERVER: IP адрес клиента: %s\n", inet_ntoa(clientAddr.sin_addr));
    printf("SERVER: PORT клиента: %d\n", ntohs(clientAddr.sin_port));
    printf("SERVER: Длина сообщения - %d\n", msgLength);
    printf("SERVER: Сообщение: %s\n\n", buf);
    if (sendto(sockMain, &buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }
  }
}
