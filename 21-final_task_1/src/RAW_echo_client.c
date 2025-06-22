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

#define BUFLEN    255

#define CLI_PORT  1
#define SERV_IP   2
#define SERV_PORT 3
#define MSG       4

#define get_port(a, b) (a << 8) + (b & 255)

#define IFACE_NAME "wlan0\0"


int sock;
struct sockaddr_in servAddr;
char buf[BUFLEN];

void finalize( int sig ){
    memcpy( &buf[28], "END\0", strlen("END")+1);
    if (sendto(sock, buf, 20+8+strlen("END")+1, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("sendto problem \n");
      exit(1);
    }
	printf("CLIENT: End\n");
	close(sock);
	exit(0);
}

int main(int argc, char *argv[]) {

  struct sockaddr_in clientAddr;
  struct hostent *hp;
  int msgLength = 0;

  if (argc < 5) {
    printf(" Usage: ./RAW_echo_client <client_port> <server_ip_addr> <server_port> <msg>\n");
    exit(1);
  }

  uint16_t src_port = atoi(argv[CLI_PORT]);
  uint16_t dst_port = atoi(argv[SERV_PORT]);
  int buf_length = 8 + strlen(argv[MSG]);

  if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0){
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
  hp = gethostbyname(argv[SERV_IP]);
  memcpy(&servAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
  servAddr.sin_port = htons(dst_port);

  memset(&clientAddr, '\0', sizeof(clientAddr));
  clientAddr.sin_family = AF_INET;
  clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddr.sin_port = htons(src_port);


  signal( SIGINT, finalize );

  char src_ip[] = {127, 0, 0, 1};
  if(memcmp(hp->h_addr_list[0], src_ip, 4)) // dst ip isn't local
  {
    struct ifreq ifr;
    memcpy(ifr.ifr_name, IFACE_NAME, strlen(IFACE_NAME)+1);
    ioctl( sock, SIOCGIFADDR, &ifr ); // ifr_ifindex
    char src_ip_str[16];
    size_t src_ip_len = strlen(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    memcpy(src_ip_str, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), src_ip_len);
    printf("%s addr = %s\n", IFACE_NAME, src_ip_str);
    int j = 0;
    for(int i = 0; i < 4; i++)
	{
		src_ip[i] = atoi(src_ip_str + j);
		for(;src_ip_str[j] && src_ip_str[j]!= '.' ;j++);
		j++;
	}
  }

  // ip //////////////////////////////////////////////////////
  memset(&buf, '\0', BUFLEN);
  buf[0] = 0X45; // 4 - ipv4; 4*5 - ip header's length

  buf[6] = 0X40; // flags dont fragment
  buf[8] = 0X40; // time to live
  buf[9] = 0X11; // protocol. 17 - udp

  buf[12] = src_ip[0]; // src ip addr (auto) <------------------
  buf[13] = src_ip[1];; // src ip addr
  buf[14] = src_ip[2];; // src ip addr
  buf[15] = src_ip[3];; // src ip addr

  buf[16] = hp->h_addr_list[0][0]; // dst ip addr
  buf[17] = hp->h_addr_list[0][1]; // dst ip addr
  buf[18] = hp->h_addr_list[0][2]; // dst ip addr
  buf[19] = hp->h_addr_list[0][3]; // dst ip addr

  // udp /////////////////////////////////////////////////////
  buf[20] = src_port>>8;
  buf[21] = src_port;

  buf[22] = dst_port>>8;
  buf[23] = dst_port;

  buf[24] = buf_length>>8;
  buf[25] = buf_length;

  memcpy( &buf[28], argv[MSG], strlen(argv[MSG]));
  ////////////////////////////////////////////////////////////
  buf_length += 20;


  char recv_buf[BUFLEN];
  int iters = argc == 6? atoi(argv[5]) : rand()%100000;

  int i = 0;
  for (; i < iters; ++i)
  {
    if (sendto(sock, buf, buf_length, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("sendto problem \n");
      exit(1);
    }

    uint16_t tmp_src_port = get_port(buf[20], buf[21]);;
    uint16_t tmp_dst_port = get_port(buf[22], buf[23]);

    printf("CLIENT: sent packet srcPort=%d dstPort=%d\n", tmp_src_port, tmp_dst_port);
    printf("        srcIp=%u.%u.%u.%u ", 255&buf[12], 255&buf[13], 255&buf[14], 255&buf[15]);
    printf(" dstIp=%u.%u.%u.%u\n", 255&buf[16], 255&buf[17], 255&buf[18], 255&buf[19]);
    printf("        '%s'\n", buf+28);

    uint16_t recv_dst_port = 0, recv_src_port;
    while(recv_dst_port!= src_port)
    {
        memset(recv_buf, 0, BUFLEN);
        int length = sizeof(clientAddr);
        if ((msgLength = recvfrom(sock, recv_buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0)
	{
            perror("recvfrom problem \n");
            exit(1);
        }

        recv_src_port = get_port(recv_buf[20], recv_buf[21]);
        recv_dst_port = get_port(recv_buf[22], recv_buf[23]);
    }

    printf("CLIENT: got packet srcPort=%u dstPort=%u\n", recv_src_port, recv_dst_port);
    printf("        srcIp=%u.%u.%u.%u ", 255&recv_buf[12], 255&recv_buf[13], 255&recv_buf[14], 255&recv_buf[15]);
    printf(" dstIp=%u.%u.%u.%u\n", 255&recv_buf[16], 255&recv_buf[17], 255&recv_buf[18], 255&recv_buf[19]);
    printf("        '%s'\n\n", recv_buf+28);
    sleep(1);
  }
  printf("iterations %d i %d\n", iters, i);

  finalize(0);
  close(sock);
  exit(0);
}
