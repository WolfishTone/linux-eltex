#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig_num)
{
    printf("handler got %d signal\n", sig_num);
}

int main()
{
    printf("pid = %d\n", getpid());
    struct sigaction new_action;

    new_action.sa_handler = handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    if(sigaction(10, &new_action, NULL) == -1)
    {
        perror("sigaction failed\n");
        exit(1);
    }
    while(1)
        sleep(10);
    exit (0);
}