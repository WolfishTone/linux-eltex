// #include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>



#define SHM_SIZE 10

int main ()
{
    char sem_name[] = "/my_sem";
    int nsems = 1;
    int proj_id = 10;

    key_t sem_key = ftok(sem_name, proj_id); 
    int sem = semget(sem_key, nsems, IPC_EXCL | IPC_CREAT | S_IRUSR | S_IWUSR);
    if(sem < 0)
    {
        perror("проблема при cоздании набора семафоров\n");
        exit(1);
    }

    struct sembuf lock = {0, -1, 0};
    // struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}};

    struct semid_ds semid_ds;

    char shm_name[] = "/my_shm";

    key_t shm_key = ftok(shm_name, proj_id); 
    int shm = shmget (shm_key, SHM_SIZE, IPC_EXCL | IPC_CREAT | S_IRUSR | S_IWUSR);
    if(shm < 0)
    {
        perror("проблема при создании разделяемой памяти\n");
        exit(1);
    }

    // get pointer to shared memory (attach shared memory)
    char *shm_ptr = shmat(shm, 0, 0);
    if (shm_ptr == (char *)-1) 
    {
        perror("проблема при присоединении разделяемой памяти\n");
        exit(1);
    }


    memcpy(shm_ptr, "Hi!\0", 4);
    printf("send message: Hi!\n");

    semop(sem, &lock, 1);
    char msg[SHM_SIZE];
    memcpy(msg, shm_ptr, SHM_SIZE);
    printf("recv message: %s\n", msg);
    
    


    // detach shared memory
    if (shmdt(shm_ptr) < 0) 
    {
        perror("проблема при отсоединении разделяемой памяти\n");
        exit(1);
    }

    // delete shared memory
    if(shmctl(shm, IPC_RMID, NULL) < 0)
    {
        perror("проблема при закрытии набора семафоров\n");
        exit(1);
    }

    // delete semaphore
    if(semctl(sem, 0, IPC_RMID, NULL) < 0)
    {
        perror("проблема при закрытии набора семафоров\n");
        exit(0);
    }

    exit(0);
}