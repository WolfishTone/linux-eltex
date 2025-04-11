#include <signal.h>         
#include <sys/syscall.h>    
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("pid = %d\n", getpid());

    sigset_t set;
    int sig_num;
    sigemptyset(&set);
    sigaddset(&set, 10);

    if(sigprocmask(SIG_BLOCK, &set, NULL) == -1)
    {
        perror("sigprocmask error\n");
        exit(1);
    }

    while(1)
    {
        if(sigwait(&set, &sig_num) != 0)
        {
            perror("sigprocmask error\n");
            exit(1);
        }
        printf("got %d signal\n", sig_num);
    }
    exit (0);
}