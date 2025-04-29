#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 

#define MSG_SIZE 10


int main()
{
    char shm_name[] = "/my_shm";
    char sem_name[] = "/my_sem";
    
    int shm = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(shm == -1)
	{
		perror("проблема при создании разделяемой памяти\n");
		exit(1);
	}
    if(ftruncate(shm, MSG_SIZE) < 0)
    {
        perror("проблема при установке размера разделяемой памяти\n");
		exit(1);
    }
    void *shm_addr = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
	if (shm_addr == MAP_FAILED)
	{
		perror("проблема при получении указателя на разделяемую памя");
		exit(1);
	}

    sem_t* sem = sem_open(sem_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);// locked
    if(sem == SEM_FAILED)
    {
        perror("проблема при создании семафорa\n");
        exit(1);
    }

    char shm_msg[MSG_SIZE];

    // put msg
    memcpy((char*)shm_addr, "Hi!\0", 4);
    printf("send message: Hi!\n");

    sem_wait(sem);
    
    // get answer
    memcpy(shm_msg, (char*)shm_addr, MSG_SIZE);
    printf("recv message: %s\n", shm_msg);

    close(shm);
    sem_close(sem);
    shm_unlink(shm_name);
    sem_unlink(sem_name);
    exit(1);

}