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
#include <signal.h>

#include <unistd.h>


#define BUFLEN 255
#define SOCKET_NAME_SERVER "/tmp/test1_local_dgram_server.socket\0"
#define SOCKET_NAME_CLIENT "/tmp/test1_local_dgram_client.socket\0"

void handler(int sig_num)
{
  printf("SERVER: Пересылка завершена. Счастливо оставаться. \n");
  unlink(SOCKET_NAME_SERVER);
  exit(0);
}

int main() {

  signal(SIGINT, handler);


  unlink(SOCKET_NAME_SERVER);
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

  int l = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);
  if (bind(sockMain, (struct sockaddr *)&servAddr, l)) {
    perror("Связывание сервера неудачно.");
    exit(1);
  }

  for (;;) {
    length = sizeof(clientAddr);
    
     char buf[BUFLEN];
    if ((msgLength = recvfrom(sockMain, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
      perror("Проблемы с recvfrom . ");
      exit(1);
    }

    printf ("SERVER: успешное получение сообщения длиной %d: %s\n", msgLength, buf) ;

    length = sizeof(clientAddr);
    if ((msgLength = sendto(sockMain, buf, msgLength, 0, (struct sockaddr *)&clientAddr, length)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }
    printf ("SERVER: успешная отправка сообщения длиной %d: %s\n\n", msgLength, buf) ;
  }
}
