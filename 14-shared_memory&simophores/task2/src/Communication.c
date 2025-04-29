#include <./Communication.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <time.h>


double wtime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec; // + ts.tv_nsec;
}

static long bcast_msg_count = 0; // to get unique msg_id for bcast msgs

void print_msg(struct Msg msg)
{
    printf("[");
    if(msg.type == Str_msg)
        printf("Str_msg");
    if(msg.type == Login_msg)
    printf("Login_msg");
    if(msg.type == Logout_msg)
    printf("Logout_msg");
    printf("]");
    printf("'%s'->%ld{#%d}\n", msg.data, msg.msg_id, msg.user_pid);
}

// if sem_val > 0 - block
void Sem_lock(int sem_id, int sem_num)
{
    // struct sembuf lock[2] = {{sem_num, 0, 0}, {sem_num, 1, 0}};
    // semop(sem_id, lock, 2);
    struct sembuf lock = {sem_num, 1, 0};
    semop(sem_id, &lock, 1);
}

void Sem_unlock(int sem_id, int sem_num)
{
    struct sembuf unlock = {sem_num, -1, 0};
    semop(sem_id, &unlock, 1);
}

void Sem_wait(int sem_id, int sem_num)
{
    struct sembuf wait = {sem_num, 0, 0};
    semop(sem_id, &wait, 1);
}

// create semaphore
enum ReturnValues Sem_create(char *file_name, int nsems, _Bool is_private ,int proj_id, int flags, int *sem)
{
    key_t sem_key;
    if(!is_private)
    {
        int fd = open(file_name, O_CREAT, S_IRWXU);
        if (fd == -1)
            return cant_create_file;
        close(fd);
        sem_key = ftok(file_name, proj_id); 
    }
    else
        sem_key = IPC_PRIVATE; 
    *sem = semget(sem_key, nsems, IPC_EXCL | IPC_CREAT | flags);
    if(*sem < 0)
        return cant_create_sem;      
    return success;
}

// for server
enum ReturnValues Sem_init(int msg_sem, int local_sem, int bcast_sem, int login_sem)
{
    // unlock 0 mem
    if(semctl (msg_sem, msg_sem_MEM, SETVAL, 0) < 0) 
        return cant_set_sem_val;
    // lock 1 control
    if(semctl (msg_sem, msg_sem_IS_SEND, SETVAL, 1) < 0)
        return cant_set_sem_val;
    // unlock 0 user msg send blockage
    if(semctl (msg_sem, msg_sem_BLOCK_CLI_SEND, SETVAL, 0) < 0)
        return cant_set_sem_val;

    // lock bcast thread
    if(semctl (local_sem, local_sem_WAKE_UP_BCAST, SETVAL, 1) < 0)
        return cant_set_sem_val;
    // lock user list thread
    if(semctl (local_sem, local_sem_WAKE_UP_LOGIN, SETVAL, 1) < 0)
        return cant_set_sem_val;
    
    // unlock 0 mem
    if(semctl (bcast_sem, bcast_sem_MEM, SETVAL, 0) < 0)
        return cant_set_sem_val;
    // unlock bcast conut
    if(semctl (bcast_sem, bcast_sem_COUNT, SETVAL, 0) < 0)
        return cant_set_sem_val;
    // lock bcast client thread 
    if(semctl (bcast_sem, bcast_sem_BCAST_START, SETVAL, 1) < 0)
        return cant_set_sem_val;

    // unlock 0 mem
    if(semctl (login_sem, login_sem_MEM, SETVAL, 0) < 0)
        return cant_set_sem_val;
    // lock bcast client thread 
    if(semctl (login_sem, login_sem_LOGIN_START, SETVAL, 1) < 0)
        return cant_set_sem_val;
    
    if(semctl (login_sem, login_sem_IS_RECV, SETVAL, 1) < 0)
        return cant_set_sem_val;

    return success;
}

enum ReturnValues Sem_init_cli(int local_sem)
{
    if(semctl (local_sem, local_cli_sem_IS_LOGIN, SETVAL, 1) < 0)
        return cant_set_sem_val;
    return success;  
}

// open semaphore
enum ReturnValues Sem_open(char *file_name, int nsems, int proj_id, int flags, int *sem)
{
    key_t sem_key = ftok(file_name, proj_id); 
    *sem = semget(sem_key, nsems, IPC_EXCL | flags);
    if(*sem < 0)
        return cant_create_sem;
    return success;
}

enum ReturnValues Sem_delete (int sem)
{
    if(semctl(sem, 0, IPC_RMID, NULL) < 0)
        return cant_delete_sem;
    return success;
}

// create shared memory
enum ReturnValues Shm_create(char *file_name, int proj_id, int flags, int *shm, struct Msg **shm_ptr)
{
    int fd = open(file_name, O_CREAT, S_IRWXU);
	if (fd == -1)
		return cant_create_file;
	close(fd);

    key_t shm_key = ftok(file_name, proj_id); 
    *shm = shmget (shm_key, sizeof(struct Msg), IPC_EXCL | IPC_CREAT | flags);
    if(*shm < 0)
        return cant_create_shm;
    // get pointer to shared memory (attach shared memory)
    *shm_ptr = shmat(*shm, 0, 0);
    if ((long) *shm_ptr < 0) 
        return cant_attach_shm;
    return success;
}


// flags - peremitions
enum ReturnValues Shm_open(char *file_name, int proj_id, int flags, int *shm, struct Msg **shm_ptr)
{
    key_t shm_key = ftok(file_name, proj_id); 
    *shm = shmget (shm_key, sizeof(struct Msg), IPC_EXCL | flags);
    if(*shm < 0)
        return cant_open_shm;
    // get pointer to shared memory (attach shared memory)
    *shm_ptr = shmat(*shm, 0, 0);
    if ((long) *shm_ptr < 0) 
        return cant_attach_shm;
    return success;
}

// detach shared memory
enum ReturnValues  Shm_detach(struct Msg *shm_ptr)
{
    if (shmdt(shm_ptr) < 0) 
        return cant_detach_shm;
    return success;
}

// delete shared memory
enum ReturnValues  Shm_delete(int shm)
{
    if(shmctl(shm, IPC_RMID, NULL) < 0)
        return cant_delete_shm;
    return success;

}


// send msg
enum ReturnValues send_msg(int msg_sem, struct Msg *shm_ptr, struct Msg* msg)
{
    // wait for server allow sending msg
    Sem_wait(msg_sem, msg_sem_BLOCK_CLI_SEND);

    Sem_wait(msg_sem, msg_sem_MEM);
    // block mem
    Sem_lock(msg_sem, msg_sem_MEM);
    // send
    struct Msg * ret = memcpy(shm_ptr, msg, sizeof(struct Msg));
    // unlock unblock mem
    Sem_unlock(msg_sem, msg_sem_MEM);
    // unlock server to read sended msg
    Sem_unlock(msg_sem, msg_sem_IS_SEND);
    if(!ret)
        return snd_error;
    return success;
}

// recive msg from client
enum ReturnValues recv_msg(int msg_sem, struct Msg *shm_ptr, struct Msg* msg)
{
    // waiting for client unlock server
    // Sem_lock(msg_sem, msg_sem_IS_SEND);
    Sem_wait(msg_sem, msg_sem_IS_SEND);
    // block mem
    Sem_wait(msg_sem, msg_sem_MEM);
    Sem_lock(msg_sem, msg_sem_MEM);
    // recv
    struct Msg * ret = memcpy(msg, shm_ptr, sizeof(struct Msg));
    // unlock mem
    Sem_unlock(msg_sem, msg_sem_MEM);
    Sem_lock(msg_sem, msg_sem_IS_SEND);
    if(!ret)
        return rcv_error;
    return success;
}


// send bcast message
enum ReturnValues bcast_msg (int msg_sem, int bcast_sem, struct Msg *shm_ptr, int users_num, struct Msg* msg)
{
    if(!users_num)
        return success;
    // block user msg sends until bcast is over
    Sem_lock(msg_sem, msg_sem_BLOCK_CLI_SEND);
    // block bcast mem
    Sem_wait(bcast_sem, bcast_sem_MEM);
    Sem_lock(bcast_sem, bcast_sem_MEM); 
    // send msg
    msg->msg_id = ++bcast_msg_count;/////////////////////////////////////////////////////////////////
    struct Msg * ret = memcpy(shm_ptr, msg, sizeof(struct Msg));
    // unblock bcast mem
    Sem_unlock(bcast_sem, bcast_sem_MEM); 
    // sent sem count to user num 
    if(semctl(bcast_sem, bcast_sem_COUNT, SETVAL, users_num) < 0)
        return cant_set_sem_val;

    // wake up clients bcast threads
    Sem_unlock(bcast_sem, bcast_sem_BCAST_START);
    // waiting for all users get msg
    Sem_wait(bcast_sem, bcast_sem_COUNT);
    // block client bcast thread
    Sem_lock(bcast_sem, bcast_sem_BCAST_START);
    // unblock user msg sends
    Sem_unlock(msg_sem, msg_sem_BLOCK_CLI_SEND);

    if(!ret)
        return snd_error;
    printf("\t%s[bcast_msg] success bcast (%g)%s ", YELLOW, wtime(), DEFAULT); print_msg(*msg);
    return success;
}

enum ReturnValues get_bcast_msg (int bcast_sem, struct Msg *shm_ptr, struct Msg* msg, long last_msg_id)
{
    Sem_wait(bcast_sem, bcast_sem_BCAST_START);
    
    // block bcast mem
    Sem_wait(bcast_sem, bcast_sem_MEM);
    Sem_lock(bcast_sem, bcast_sem_MEM); 
    // send msg
    struct Msg * ret = memcpy(msg, shm_ptr, sizeof(struct Msg));
    // unblock bcast mem
    Sem_unlock(bcast_sem, bcast_sem_MEM); 
    // decrease bcast count
    if(last_msg_id != msg->msg_id)
        Sem_unlock(bcast_sem, bcast_sem_COUNT);
    // sleep until bcast ends
    Sem_wait(bcast_sem, bcast_sem_COUNT);

    if(!ret)
        return rcv_error;
    return success;
}

enum ReturnValues send_user_list(int login_sem, struct Msg* shm_ptr, struct UsersList *users_list)
{
    struct Msg msg;
	struct User* tmp = users_list->head;
    struct Msg * ret;
	msg.type = Login_msg;
	while(1)
	{
        if(tmp)
        {
            msg.user_pid = tmp->user_id;
            sprintf(msg.data, "%s", tmp->user_name);
        }
        else
            msg.user_pid = 0;

        // block login mem
        Sem_wait(login_sem, login_sem_MEM); 
        Sem_lock(login_sem, login_sem_MEM); 
        // send msg
        ret = memcpy(shm_ptr, &msg, sizeof(struct Msg));
        // unblock login mem
        Sem_unlock(login_sem, login_sem_MEM); 

        // start user login thread 
        Sem_unlock(login_sem, login_sem_LOGIN_START);

        // wait until user recive msg
        Sem_wait(login_sem, login_sem_IS_RECV);
        Sem_lock(login_sem, login_sem_IS_RECV);

        if(!ret)
            return snd_error;
    
        if(!tmp)
            return success;
        
        tmp = tmp->next;
	}
    
	return success;
}

enum ReturnValues recv_user_list(int login_sem, struct Msg* shm_ptr, struct UsersList *users_list)
{
    while(1)
    {
        Sem_wait(login_sem, login_sem_LOGIN_START);
        Sem_lock(login_sem, login_sem_LOGIN_START);
        struct Msg login_msg;

        // block login mem
        Sem_wait(login_sem, login_sem_MEM); 
        Sem_lock(login_sem, login_sem_MEM); 
        if( !memcpy(&login_msg, shm_ptr, sizeof(struct Msg)) )
            return rcv_error;

        // unblock login mem
        Sem_unlock(login_sem, login_sem_MEM);

        // sleep(2); 
        Sem_unlock(login_sem, login_sem_IS_RECV);

        if(login_msg.user_pid == 0)
            return success;

        enum ReturnValues ret =  UsersList_add(users_list, login_msg.user_pid, login_msg.data);
        if(ret != success && ret != user_exists)
            return ret;
    }
    return success;
}