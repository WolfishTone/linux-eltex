#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>


#define MSG_SIZE 10

int main()
{
    char shm_name[] = "/my_shm";
    char sem_name[] = "/my_sem";

    int shm = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    if(shm == -1)
	{
		perror("проблема при открытии разделяемой памяти\n");
		exit(1);
	}
    void *shm_addr = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
	if (shm_addr == MAP_FAILED)
	{
		perror("проблема при получении указателя на разделяемую памя");
		exit(1);
	}

    sem_t * sem = sem_open(sem_name, O_RDWR);    
    if(sem == SEM_FAILED)
    {
        perror("проблема при открытии семафорa\n");
        exit(1);
    }


    char shm_msg[MSG_SIZE];

    // get answer
    memcpy(shm_msg, (char*)shm_addr, 7);
    printf("recv message: %s\n", shm_msg);

    memcpy((char*)shm_addr, "Hello!\0", 7);
    printf("send message: Hello!\n");
    
    sem_post(sem);

    close(shm);
    sem_close(sem);
    exit(1);

}