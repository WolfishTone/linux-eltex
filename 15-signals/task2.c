#include <signal.h>         
#include <sys/syscall.h>    
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("pid = %d\n", getpid());

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 2);

    if(sigprocmask(SIG_BLOCK, &set, NULL) == -1)
    {
        perror("sigprocmask error\n");
        exit(1);
    }
    while(1)
        sleep(10);
    exit (0);
}