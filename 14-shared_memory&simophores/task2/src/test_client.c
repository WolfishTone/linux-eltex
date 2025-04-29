#include <./Communication.h>
#include <./Msg_history.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> // for getpid
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>


pid_t pid;
int msg_sem;   // (mem) - exclusive access to shm_msg
               // (is_send) - unlock server to get clients msgs 
               // (block_cli_send) - block clients send while server bcast msgs
int bcast_sem; // (mem) exclusive access to shm_bcast
               // (count) block bcast until all users have read the message
               // (bcast_start) make wake up client bcast thread and make it sleep after bcast 

int shm_msg, shm_bcast;
struct Msg * shm_msg_ptr;
struct Msg *shm_bcast_ptr;

struct UsersList *users_list = NULL; // list of online users
struct Msg_history* msg_hist; // history of messages

void finalize(int ret_code)
{
	// bcast ending message
    struct Msg logout_msg = {.data= "Logout", .type= Logout_msg, .user_pid= pid};
    send_msg(msg_sem, shm_msg_ptr, &logout_msg);


	// detach shared_memory
    Shm_detach(shm_msg_ptr);
    Shm_detach(shm_bcast_ptr);
	
	UsersList_free(users_list);
    Msg_history_free(msg_hist);
	printf("goodbye!\n");
	exit(ret_code);
}

void handler()
{
	finalize(0);
}

void* bcast_thread(void*) // sending bcast msgs
{
    printf("bcast server thread created\n");
	struct Msg msg_bcast;
	enum ReturnValues ret;

    while(1)
    {
        printf("-----------------------------------wait for unblock bcast thread\n");
        Sem_wait(bcast_sem, bcast_sem_BCAST_START);
        printf("-----------------------------------unlock\n");

        // sleep while bcast_sem_bcast_start
        ret = get_bcast_msg (bcast_sem, shm_bcast_ptr, &msg_bcast);
        if(ret == rcv_error)
        {
            printf("failed to recieve bcast msg [%ld]'%s' -> %d", msg_bcast.type, msg_bcast.data, msg_bcast.user_pid);
            perror(" ");
        }
        
        if(msg_bcast.type == Str_msg)
        {
            char find_user_name[USER_NAME_SIZE];
            ret = UsersList_find_user_by_id(*users_list, msg_bcast.user_pid, find_user_name); 
            // add msg to msg list
            ret = Msg_history_add(msg_hist, find_user_name, msg_bcast.data);
            if(ret == NULL_ptr)
            {
                perror("can't add message to message list ");
                finalize(1);
            }
            printf("recieved msg\n");
            Msg_history_print(*msg_hist);
        }
        if(msg_bcast.type == Login_msg)
        {
            // add user
            ret =  UsersList_add(users_list, msg_bcast.user_pid, msg_bcast.data);
			if(ret == invalid_user_id)
			{
				printf("invalid user id\n");
				finalize(1);
			}
			if(ret == malloc_err)
			{
				printf("malloc err\n");
				finalize(1);
			}
			if(ret == user_exists)
			{
				printf("user exists\n");
				finalize(1);
			}
            printf("added new user\n");
            UsersList_print(*users_list);

        }
        if(msg_bcast.type == Logout_msg)
        {
            if(msg_bcast.user_pid == Sever_death)
                pthread_exit(0);
            // delete user
            ret = UsersList_delete_by_user_id(users_list, msg_bcast.user_pid);
            if(ret == NULL_ptr)
            {
                perror("cant delete user from user_list ");
                finalize(1);
            }
            printf("deleted user\n");
            UsersList_print(*users_list);

        }
        printf("get bcast msg "); print_msg(msg_bcast);
        // // sleep until bcast ends
        // Sem_wait(bcast_sem, bcast_sem_COUNT);
        // block bcast_thread after bcast
    }
}


int main(int argc, char** argv)
{
    printf("server pid = %d\n", getpid());
    // sigint handle
	struct sigaction new_action;
    new_action.sa_handler = handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
	if(sigaction(2, &new_action, NULL) == -1)
    {
        perror("sigaction failed\n");
        exit(1);
    }

    // open semaphores
    enum ReturnValues ret = Sem_open(MSG_SEM_FILE_NAME, msg_sem_NUM, MSG_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &msg_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create msg_sem ");
        exit(1);
    }
    ret = Sem_open(BCAST_SEM_FILE_NAME, bcast_sem_NUM, BCAST_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &bcast_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create bcast_sem ");
        exit(1);
    }

    // 
    ret = Shm_open(SHM_MSG_FILE_NAME, SHM_MSG_PROJ_ID, S_IRUSR | S_IWUSR, &shm_msg, &shm_msg_ptr);
    if(ret == cant_open_shm)
    {
        perror("can't open msg shared memory ");
        exit(1);
    }
    if(ret == cant_attach_shm)
    {
        perror("can't attach msg shared memory ");
        exit(1);
    }
    ret = Shm_open(SHM_BCAST_FILE_NAME, SHM_BCAST_PROJ_ID, S_IRUSR | S_IWUSR, &shm_bcast, &shm_bcast_ptr);
    if(ret == cant_open_shm)
    {
        perror("can't open bcast shared memory ");
        exit(1);
    }
    if(ret == cant_attach_shm)
    {
        perror("can't attach bcast shared memory ");
        exit(1);
    }


    // user list
    users_list = UsersList_create();


    // history of messages
    msg_hist =  Msg_history_create();

    pthread_t thread;
    if (pthread_create(&thread, NULL, (void *) bcast_thread, (void*) NULL) != 0) {
        perror("can't create bcast thread ");
        finalize(1);
    }
    if (pthread_detach(thread) != 0) {
        perror("can't detach bcast thread ");
        finalize(1);
    }

    // cli_name checking, sending
    pid = getpid();
    struct Msg msg; 
    msg.type = Login_msg;
    msg.user_pid = pid;
    if(argc == 2) {    
        if(strlen(argv[1]) > USER_NAME_SIZE) {
            printf("Max cli_name size = %d\n", USER_NAME_SIZE);
            finalize(1);
        }
        sprintf(msg.data, "%s", argv[1]);   
    }
    else // if user didn't get uname  
        sprintf(msg.data, "user_%d", pid);
    
    printf("sended [%ld]'%s' -> %d\n", msg.type, msg.data, msg.user_pid);

    if(send_msg(msg_sem, shm_msg_ptr, &msg) == snd_error)
    {
        perror("can't send Login_msg ");
        finalize(1);
    }

    msg.type = Str_msg;

    while (1)
    {
        
        scanf("%s", msg.data);
        if(send_msg(msg_sem, shm_msg_ptr, &msg) == snd_error)
        {
            perror("can't send Str_msg ");
        }
        printf("sended "); print_msg(msg);
        int cancel_ret = pthread_cancel(thread);
        if(cancel_ret != ESRCH)
            finalize(0);

    }
}