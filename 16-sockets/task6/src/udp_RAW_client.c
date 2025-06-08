#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define BUFLEN 255
#define MAC_SIZE 6



int main(int argc, char *argv[]) {

  int sock;
  struct sockaddr_ll servAddr, clientAddr;

  if (argc < 3) {
    printf(" ВВЕСТИ udpclient порт сообщение\n");
    exit(1);
  }

  if ((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
    perror("He могу получить socket\n");
    exit(1);
  }

  char interface_name[] = "wlan0\0";
  uint8_t src_mac[MAC_SIZE] = {0x80, 0x38, 0xfb, 0xcd, 0x9f, 0x65};
  uint8_t dst_mac[MAC_SIZE] = {0x78, 0x92, 0x9c, 0x64, 0x56, 0xde};

  struct ifreq ifr;
  memcpy(ifr.ifr_name, interface_name, strlen(interface_name)+1);
  ioctl( sock, SIOCGIFINDEX, &ifr ); // ifr_ifindex
  printf("interface %s has index %d\n", ifr.ifr_name, ifr.ifr_ifindex);


  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sll_family = AF_PACKET;
  //sll_addr - MAC адрес назначения
  memcpy(servAddr.sll_addr, dst_mac, MAC_SIZE);
  //sll_halen = 6 - длинна мак адреса
  servAddr.sll_halen = 6;
  //sll_ifindex - индекс адаптора сети
  servAddr.sll_ifindex = ifr.ifr_ifindex;

  memset(&clientAddr, 0, sizeof(clientAddr));
  clientAddr.sll_family = AF_PACKET;
  //sll_addr - MAC адрес назначения
  memcpy(clientAddr.sll_addr, src_mac, MAC_SIZE);
  //sll_halen = 6 - длинна мак адреса
  clientAddr.sll_halen = 6;
  //sll_ifindex - индекс адаптора сети
  clientAddr.sll_ifindex = ifr.ifr_ifindex;



  int msgLength = 0;
  printf("CLIENT: Готов к пересылке.\n");
  uint8_t buf[BUFLEN];

  // Ethernet II /////////////////////////////////////////////
  memcpy(buf, dst_mac, MAC_SIZE); // dst mac
  memcpy(&buf[6], src_mac, MAC_SIZE); // src mac
  buf[12] = 0X08;
  buf[13] = 0X00;

  // ip //////////////////////////////////////////////////////
  uint16_t total_length = 20 + 8 + strlen(argv[2]) + 1;
  buf[14] = 0X45; // 4 - ipv4; 4*5 - ip header's length

  buf[15] = 0X00; // ds

  buf[16] = total_length>>8; // total length (auto) ip_header+udp_header+data
  buf[17] = total_length;    // total length

  buf[18] = 0X00; // Identification (auto)
  buf[19] = 0X00; // Identification

  buf[20] = 0X40; // flags dont fragment
  buf[21] = 0X00; // fragment offset
  buf[22] = 0X40; // time to live
  buf[23] = 0X11; // protocol. 17 - udp

  buf[24] = 0X00; // header checksum (auto)
  buf[25] = 0X00; // header checksum

  buf[26] = 192; // src ip addr (auto)
  buf[27] = 168; // src ip addr
  buf[28] = 1; // src ip addr
  buf[29] = 102; // src ip addr

  buf[30] = 192; // dst ip addr
  buf[31] = 168; // dst ip addr
  buf[32] = 1; // dst ip addr
  buf[33] = 202; // dst ip addr

  uint32_t csum =0;
  for(int i= 0; i< 20; i+=2)
    csum+= (buf[14+i]<<8) + buf[15+i];
  uint16_t tmp = csum >> 16;
  csum = (csum & 0xffff) + tmp;
  csum = ~csum;

  buf[24] = csum>>8; // header checksum
  buf[25] = csum; // header checksum





  // udp /////////////////////////////////////////////////////
  uint16_t src_port = 7777;
  uint16_t dst_port = atoi(argv[1]);
  uint16_t buf_length = 8 + strlen(argv[2])+1;

  buf[34] = src_port>>8;
  buf[35] = src_port;

  buf[36] = dst_port>>8;
  buf[37] = dst_port;

  buf[38] = buf_length>>8;
  buf[39] = buf_length;

  buf[40] = 0;
  buf[41] = 0;

  memcpy( &buf[42], argv[2], strlen(argv[2]));
  ////////////////////////////////////////////////////////////
  buf_length += 20 + 14;
  buf[buf_length-1] = '\0';


  uint8_t recv_buf[BUFLEN];
  for (int i = 1; i < 10; ++i) { //
    if (sendto(sock, buf, buf_length, 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
      perror("Проблемы с sendto . \n");
      exit(1);
    }

    uint16_t tmp_src_port = (buf[34] << 8) + buf[35];
    uint16_t tmp_dst_port = (buf[36] << 8) + buf[37];

    printf("CLIENT: send packet srcPort=%u dstPort=%u\n", tmp_src_port, tmp_dst_port);
    printf(" srcIp=%u.%u.%u.%u ", buf[26],  buf[27],  buf[28],  buf[29]);
    printf(" dstIp=%u.%u.%u.%u ", buf[30],  buf[31],  buf[32],  buf[33]);
    printf(" '%s'\n", buf+42);
    printf("\n\n");

    uint16_t recv_dst_port = 0, recv_src_port;
    while(recv_dst_port!= src_port)
    {
        memset(recv_buf, 0, BUFLEN);
        int length = sizeof(clientAddr);
        if ((msgLength = recvfrom(sock, recv_buf, BUFLEN, 0, (struct sockaddr *)&servAddr, (socklen_t *)&length)) < 0) {
            perror("Плохой socket клиента.");
            exit(1);
        }
        recv_src_port = (recv_buf[34] << 8) + recv_buf[35];
        recv_dst_port = (recv_buf[36] << 8) + recv_buf[37];
    }
    
    printf("CLIENT: get packet srcPort=%u dstPort=%u\n", recv_src_port, recv_dst_port);
    printf(" srcIp=%u.%u.%u.%u ", recv_buf[26],  recv_buf[27],  recv_buf[28],  recv_buf[29]);
    printf(" dstIp=%u.%u.%u.%u ", recv_buf[30],  recv_buf[31],  recv_buf[32],  recv_buf[33]);
    printf(" '%s'\n", recv_buf+42);
    printf("\n\n");

    sleep(2);
  }

  printf("CLIENT: Пересылка завершена. Счастливо оставаться. \n");
  close(sock);
  exit(0);
}
