#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        printf("Usage: ./killer <pid> <signal_num>\n");
        exit(1);
    }
    if(kill((pid_t)atoi(argv[1]), atoi(argv[2])) == -1)
    {
        perror("sending signal failed\n");
        exit(1);
    }
    exit(0);
}