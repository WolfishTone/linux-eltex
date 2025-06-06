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

  if (argc < 4) {
    printf(" ВВЕСТИ udpclient имя_хоста порт сообщение\n");
    exit(1);
  }

  if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0){
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
  clientAddr.sin_port = htons(7777);



  /*if (bind(sock, (struct sockaddr *)&clientAddr, sizeof(clientAddr))) {
    perror("Клиент не получил порт.");
    exit(1);
  }*/

  int msgLength = 0;
  printf("CLIENT: Готов к пересылке.\n");

  uint16_t src_port = 7777;
  uint16_t dst_port = atoi(argv[2]);
  uint16_t buf_length = 8 + strlen(argv[3]);

  char buf[BUFLEN];
  buf[0] = src_port>>8;
  buf[1] = src_port;

  buf[2] = dst_port>>8;
  buf[3] = dst_port;

  buf[4] = buf_length>>8;
  buf[5] = buf_length;

  buf[6] = 0;
  buf[7] = 0;

  memcpy( &buf[8], argv[3], strlen(argv[3]));



  char recv_buf[BUFLEN];
  for (int i = 1; i < 10; ++i) { //
    if (sendto(sock, buf, buf_length, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }

     printf("CLIENT: send packet srcPort=%u dstPort=%u\n", src_port, dst_port);
     printf(" '%s'\n", buf+8);
     printf("\n");

    uint16_t recv_dst_port = 0, recv_src_port;
    while(recv_dst_port!= src_port)
    {
        memset(recv_buf, 0, BUFLEN);
        int length = sizeof(clientAddr);
        if ((msgLength = recvfrom(sock, recv_buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
            perror("Плохой socket клиента.");
            exit(1);
        }

        recv_src_port = (recv_buf[20] << 8) + recv_buf[21];
        recv_dst_port = (recv_buf[22] << 8) + recv_buf[23];
    }
    
    printf("CLIENT: got packet srcPort=%u dstPort=%u\n", recv_src_port, recv_dst_port);
    printf(" srcIp=%u.%u.%u.%u ", recv_buf[12],  recv_buf[13],  recv_buf[14],  recv_buf[15]);
    printf(" dstIp=%u.%u.%u.%u ", recv_buf[16],  recv_buf[17],  recv_buf[18],  recv_buf[19]);
    printf(" '%s'\n", recv_buf+28);
    printf("\n\n");

    sleep(2);
  }

  printf("CLIENT: Пересылка завершена. Счастливо оставаться. \n");
  close(sock);
  exit(0);
}
