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

int main()
{
    int sock;
    struct sockaddr_in clientAddr;

    if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("He могу получить socket\n");
        exit(1);
    }
    
    uint8_t buf[BUFLEN];
    memset(buf, '\0', BUFLEN);
    int msgLength, length;
    while(1)
    {
        memset(buf, '\0', BUFLEN);
        if ((msgLength = recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length)) < 0) {
            perror("Плохой socket клиента.");
            exit(1);
        }

        uint16_t srcPort = (buf[20] << 8) + buf[21];
        uint16_t dstPort = (buf[22] << 8) + buf[23];
        printf("CLIENT: got packet srcPort=%u dstPort=%u\n", srcPort, dstPort);
        printf(" srcIp=%u.%u.%u.%u ", buf[12],  buf[13],  buf[14],  buf[15]);
        printf(" dstIp=%u.%u.%u.%u ", buf[16],  buf[17],  buf[18],  buf[19]);
        printf(" '%s'\n", buf+28);
        printf("\n");
    }
}