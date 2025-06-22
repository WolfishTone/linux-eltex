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
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <splayTree.h>

#define BUFLEN    255
#define SERV_PORT 1
#define get_port(a, b) (a << 8) + (b & 255)
#define IFACE_NAME "wlan0\0"

struct SplayTree* clients_tree = 0;

int get_client_count (char *clients_ip, int clients_port)
{
  int count;
  clients_tree = SplayTreeInsert(clients_tree, clients_port, clients_ip);

  return ++clients_tree->count;
}

_Bool reset_client_count(char *clients_ip, int clients_port, char * msg) // check if client sent END message and reset it's count
{
  if(!strcmp(msg, "END\0"))
  {
    struct SplayTree* finded_client = SplayTreeLookup(clients_tree, clients_port, clients_ip);
    if(finded_client)
    {
      finded_client->count = 0;
      return 1; // client sent END msg
    }
  }
  return 0;
}
int sock;

void finalize( int sig ){
  SplayTreeFree(clients_tree);
  clients_tree= 0;
  close(sock);
  printf("SERVER: End.\n");
  exit(0);
}


int main(int argc, char *argv[])
{
  struct sockaddr_in servAddr, clientAddr;
  int msgLength = 0;

  if (argc < 2) {
    printf(" Usage: ./RAW_echo_server <server_port>\n");
    exit(1);
  }

  uint16_t src_port = atoi(argv[SERV_PORT]);

  if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
  {
    perror("Can't get socket ");
    exit(1);
  }

  int one = 1;
  const int *val = &one;

  if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
    perror("setsockopt() error ");
    exit(2);
  }

  memset(&servAddr, '\0', sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(src_port);


  signal( SIGINT, finalize );


  struct SplayTree* clients_tree = 0;

  sleep(10);
  char recv_buf[BUFLEN];
  while(1)
  {
    uint16_t recv_dst_port = 0, recv_src_port;
    while(recv_dst_port!= src_port)
    {
        memset(recv_buf, 0, BUFLEN);
        int length = sizeof(clientAddr);
        if ((msgLength = recvfrom(sock, recv_buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
            perror("recvfrom problem \n");
            exit(1);
        }

        recv_src_port = get_port(recv_buf[20], recv_buf[21]);
        recv_dst_port = get_port(recv_buf[22], recv_buf[23]);
    }

    char srcIp[] = {recv_buf[12], recv_buf[13], recv_buf[14], recv_buf[15]};
    char dstIp[] = {recv_buf[16], recv_buf[17], recv_buf[18], recv_buf[19]};
    uint16_t dst_port = recv_src_port;

    printf("SERVER: got packet srcPort=%u dstPort=%u\n", recv_src_port, recv_dst_port);
    printf("        srcIp=%u.%u.%u.%u ", (uint8_t)srcIp[0], (uint8_t)srcIp[1], (uint8_t)srcIp[2], (uint8_t)srcIp[3]);
    printf(" dstIp=%u.%u.%u.%u\n", (uint8_t)dstIp[0], (uint8_t)dstIp[1], (uint8_t)dstIp[2], (uint8_t)dstIp[3]);
    printf("        '%s' length %d\n", recv_buf+28, msgLength);

    _Bool is_client_end_msg = reset_client_count(dstIp, dst_port, recv_buf+28);

    if(is_client_end_msg)
    {
      printf("client's last msg\n\n");
      continue;
    }

    char msg[BUFLEN -29];
    char buf[BUFLEN];
    sprintf(msg, "%s %d", recv_buf+28, get_client_count( dstIp, dst_port));
    int buf_length = 8 + strlen(msg);

    // ip //////////////////////////////////////////////////////
    memset(&buf, '\0', BUFLEN);
    buf[0] = 0X45; // 4 - ipv4; 4*5 - ip header's length

    buf[6] = 0X40; // flags dont fragment
    buf[8] = 0X40; // time to live
    buf[9] = 0X11; // protocol. 17 - udp

    buf[12] = dstIp[0]; // src ip addr (auto)
    buf[13] = dstIp[1];; // src ip addr
    buf[14] = dstIp[2];; // src ip addr
    buf[15] = dstIp[3];; // src ip addr

    buf[16] = srcIp[0]; // dst ip addr
    buf[17] = srcIp[1]; // dst ip addr
    buf[18] = srcIp[2]; // dst ip addr
    buf[19] = srcIp[3]; // dst ip addr

    // udp /////////////////////////////////////////////////////
    buf[20] = src_port>>8;
    buf[21] = src_port;

    buf[22] = dst_port>>8;
    buf[23] = dst_port;

    buf[24] = buf_length>>8;
    buf[25] = buf_length;


    memcpy( &buf[28], msg, strlen(msg));
    ////////////////////////////////////////////////////////////
    buf_length += 20 + 1;

    memset(&clientAddr, '\0', sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    memcpy(&servAddr.sin_addr, srcIp, 4);
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(src_port);

    if (sendto(sock, buf, buf_length, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
      perror("sendto problem \n");
      exit(1);
    }

    printf("SERVER: sent packet srcPort=%u dstPort=%u\n", get_port(buf[20], buf[21]), get_port(buf[22], buf[23]));
    printf("        srcIp=%u.%u.%u.%u ", (uint8_t)buf[12], (uint8_t)buf[13], (uint8_t)buf[14], (uint8_t)buf[15]);
    printf(" dstIp=%u.%u.%u.%u\n", (uint8_t)buf[16], (uint8_t)buf[17], (uint8_t)buf[18], (uint8_t)buf[19]);
    printf("        '%s' length %d\n\n", buf+28, buf_length);
  }
}
