#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <unistd.h>


#define BUFLEN 255
#define SOCKET_NAME_SERVER "/tmp/test1_local_dgram_server.socket\0"
#define SOCKET_NAME_CLIENT "/tmp/test1_local_dgram_client.socket\0"

int main(int argc, char **argv) {
  unlink(SOCKET_NAME_CLIENT);
  int sockMain, length, msgLength;
  struct sockaddr_un servAddr, clientAddr;
  
  if ((sockMain = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
    perror("Сервер не может открыть socket.");
    exit(1);
  }
  
  memset(&servAddr, '\0', sizeof(servAddr));  
  servAddr.sun_family = AF_LOCAL;
  memcpy(servAddr.sun_path, SOCKET_NAME_SERVER, sizeof(SOCKET_NAME_SERVER));

  memset(&clientAddr, '\0', sizeof(clientAddr));
  clientAddr.sun_family = AF_LOCAL;
  memcpy(clientAddr.sun_path, SOCKET_NAME_CLIENT, sizeof(SOCKET_NAME_CLIENT));

  int l = strlen(clientAddr.sun_path) + sizeof(clientAddr.sun_family);
  if (bind(sockMain, (struct sockaddr *)&clientAddr, l)) {
    perror("Связывание клиента неудачно.");
    exit(1);
  }

  for (int i = 0;i < 10;i++) {
    length = sizeof(servAddr);

     if ((msgLength = sendto(sockMain, argv[1], sizeof(argv[1]), 0, (struct sockaddr *)&servAddr, length)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }
    printf ("CLIENT: успешная отправка сообщения длиной %d: %s\n", msgLength, argv[1]) ;

    sleep(1);

    char buf[BUFLEN];
    if ((msgLength = recvfrom(sockMain, buf, BUFLEN, 0, (struct sockaddr *)&servAddr, (socklen_t *)&length)) < 0) {
      perror("Проблемы с recvfrom . ");
      exit(1);
    }
    printf ("CLIENT: успешное получение сообщения длиной %d: %s\n\n", msgLength, buf) ;
  }

  printf("CLIENT: Пересылка завершена. Счастливо оставаться. \n");
  close(sockMain);
  unlink(SOCKET_NAME_CLIENT);
  exit(0);
}