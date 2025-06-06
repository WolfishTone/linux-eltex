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

  int one = 1;
  const int *val = &one;

  if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    perror("setsockopt() error");
    exit(2);
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



  int msgLength = 0;
  printf("CLIENT: Готов к пересылке.\n");
  char buf[BUFLEN];

  // ip //////////////////////////////////////////////////////
  buf[0] = 0X45; // 4 - ipv4; 4*5 - ip header's length

  buf[1] = 0X00; // ds

  buf[2] = 0X00; // total length (auto)
  buf[3] = 0X00; // total length

  buf[4] = 0X00; // Identification (auto)
  buf[5] = 0X00; // Identification

  buf[6] = 0X40; // flags dont fragment
  buf[7] = 0X00; // fragment offset
  buf[8] = 0X40; // time to live
  buf[9] = 0X11; // protocol. 17 - udp

  buf[10] = 0X00; // header checksum (auto)
  buf[11] = 0X00; // header checksum

  buf[12] = 127; // src ip addr (auto)
  buf[13] = 0; // src ip addr
  buf[14] = 0; // src ip addr
  buf[15] = 1; // src ip addr

  buf[16] = 127; // dst ip addr
  buf[17] = 0; // dst ip addr
  buf[18] = 0; // dst ip addr
  buf[19] = 1; // dst ip addr



  // udp /////////////////////////////////////////////////////
  uint16_t src_port = 7777;
  uint16_t dst_port = atoi(argv[2]);
  uint16_t buf_length = 8 + strlen(argv[3]);

  buf[20] = src_port>>8;
  buf[21] = src_port;

  buf[22] = dst_port>>8;
  buf[23] = dst_port;

  buf[24] = buf_length>>8;
  buf[25] = buf_length;

  buf[26] = 0;
  buf[27] = 0;

  memcpy( &buf[28], argv[3], strlen(argv[3]));
  ////////////////////////////////////////////////////////////
  buf_length += 20;
  buf[buf_length] = '\0';
  buf_length ++;


  char recv_buf[BUFLEN];
  for (int i = 1; i < 10; ++i) { //
    if (sendto(sock, buf, buf_length, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }

    uint16_t tmp_src_port = (buf[20] << 8) + buf[21];
    uint16_t tmp_dst_port = (buf[22] << 8) + buf[23];

    printf("CLIENT: send packet srcPort=%u dstPort=%u\n", tmp_src_port, tmp_dst_port);
    printf(" srcIp=%u.%u.%u.%u ", buf[12],  buf[13],  buf[14],  buf[15]);
    printf(" dstIp=%u.%u.%u.%u ", buf[16],  buf[17],  buf[18],  buf[19]);
    printf(" '%s'\n", buf+28);
    printf("\n\n");

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
