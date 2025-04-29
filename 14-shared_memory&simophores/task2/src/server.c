# include <sys/types.h>
# include <sys/ipc.h>
#include <fcntl.h>
# include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>
#include <pthread.h>
#include <./Communication.h>


int msg_sem;   // (mem) exclusive access to shm_msg
               // (is_send) - make server wake up to get clients msgs 
               // (block_cli_send) - block clients send while server bcast msgs

int local_sem; // (wake_up_bcast) - blocks and unblocks bcast thread
			   // (wake_up_login) - blocks and unblocks login thread

int bcast_sem; // (mem) exclusive access to shm_bcast
               // (count) block bcast until all users have read the message
               // (bcast_start) make wake up client bcast thread and make it sleep after bcast 

int login_sem; // (mem) exclusive access to shm_login
			   // (login_start) make wake up client login thread and make it sleep after getting user list
			   // (is_recv) confirmation of msg is recieved

int shm_msg, shm_bcast, shm_login;
struct Msg * shm_msg_ptr;
struct Msg *shm_bcast_ptr;
struct Msg *shm_login_ptr;
struct UsersList *users_list = NULL; // list of online users

void finalize(int ret_code)
{
    // bcast ending message
    struct Msg logout_msg = {.data= "Server death", .type= Logout_msg, .user_pid= Sever_death};
    enum ReturnValues ret = bcast_msg (msg_sem, bcast_sem, shm_bcast_ptr, users_list->num, &logout_msg);

    if(ret == snd_error)
    {
        printf("failed to bcast msg "); print_msg(logout_msg);
        perror("");
    }
    if(ret == cant_set_sem_val)
        perror("can't set value to semaphore ");

	// delete shared_memory
    if(Shm_detach(shm_msg_ptr) == cant_detach_shm)
    {
        perror("cant detach shared memory ");
        exit(1);
    }
    if(Shm_delete(shm_msg) == cant_delete_shm)
    {
        perror("cant delete shared memory ");
        exit(1);
    }
    if(Shm_detach(shm_bcast_ptr) == cant_detach_shm)
    {
        perror("cant detach shared memory ");
        exit(1);
    }
    if(Shm_delete(shm_bcast) == cant_delete_shm)
    {
        perror("cant delete shared memory ");
        exit(1);
    }
	if(Shm_detach(shm_login_ptr) == cant_detach_shm)
    {
        perror("cant detach shared memory ");
        exit(1);
    }
    if(Shm_delete(shm_login) == cant_delete_shm)
    {
        perror("cant delete shared memory ");
        exit(1);
    }
    // delete semaphore
    if(Sem_delete (msg_sem) == cant_delete_sem)
    {
        perror("cant delete semaphore ");
        exit(1);
    }
    if(Sem_delete (local_sem) == cant_delete_sem)
    {
        perror("cant delete semaphore ");
        exit(1);
    }
    if(Sem_delete (bcast_sem) == cant_delete_sem)
    {
        perror("cant delete semaphore ");
        exit(1);
    }
	if(Sem_delete (login_sem) == cant_delete_sem)
    {
        perror("cant delete semaphore ");
        exit(1);
    }
	
	UsersList_free(users_list);
	printf("goodbye!\n");
	exit(ret_code);
}

void handler()
{
	finalize(0);
}


void* bcast_thread(void* in) // sending bcast msgs
{
    printf("bcast server thread created\n");
	struct Msg * msg_bcast = (struct Msg *)in;
	enum ReturnValues ret;

    while(1)
    {
        //wait for unblock bcast thread
        Sem_wait(local_sem, local_sem_WAKE_UP_BCAST);
        printf("[bcast_thread] start bcast %g ", wtime()); print_msg(*msg_bcast);

        ret = bcast_msg (msg_sem, bcast_sem, shm_bcast_ptr, users_list->num, msg_bcast);

        if(ret == snd_error)
        {
            printf("[bcast_thread] failed to bcast msg [%ld]'%s' -> %d", msg_bcast->type, msg_bcast->data, msg_bcast->user_pid);
            perror("");
        }
        if(ret == cant_set_sem_val)
        {
            perror("[bcast_thread] can't set value to semaphore ");
            finalize(1);
        }
        printf("[bcast_thread] bcasted\n");
		Sem_lock(local_sem, local_sem_WAKE_UP_BCAST);
    }
    pthread_exit(0);
}

void* login_thread(void*) // sending users list
{

	while(1)
	{
		Sem_wait(local_sem, local_sem_WAKE_UP_LOGIN);
		printf("[login_thread] start user list sending\n");

		if(send_user_list(login_sem, shm_login_ptr, users_list) == snd_error)
		{
			perror("[login_thread] can't send user list ");
			finalize(1);
		}

		printf("[login_thread] user_list_sended\n");
		Sem_lock(local_sem, local_sem_WAKE_UP_LOGIN);
	}
	pthread_exit(0);
}



int main()
{
    printf("server pid = %d\n", getpid());
	// sigint handle
	struct sigaction new_action;
    new_action.sa_handler = handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
	if(sigaction(2, &new_action, NULL) == -1)
    {
        perror("sigaction failed ");
        exit(1);
    }


	// create semaphore
    enum ReturnValues ret = Sem_create(MSG_SEM_FILE_NAME, msg_sem_NUM, NAMED, MSG_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &msg_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create msg_sem ");
        exit(1);
    }
    ret = Sem_create(LOCAL_SEM_FILE_NAME, local_sem_NUM, PRIVATE, LOCAL_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &local_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create local_sem ");
        exit(1);
    }
    ret = Sem_create(BCAST_SEM_FILE_NAME, bcast_sem_NUM, NAMED, BCAST_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &bcast_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create bcast_sem ");
        exit(1);
    }
	ret = Sem_create(LOGIN_SEM_FILE_NAME, login_sem_NUM, NAMED, LOGIN_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &login_sem);
    if(ret == cant_create_sem)
    {
        perror("can't create login_sem ");
        exit(1);
    }
    if(Sem_init(msg_sem, local_sem, bcast_sem, login_sem) == cant_set_sem_val)
    {
        perror("can't init semaphores ");
        exit(1);
    }
    printf("semaphores created ");

	// create shared memory
    ret = Shm_create(SHM_MSG_FILE_NAME, SHM_MSG_PROJ_ID, S_IRUSR | S_IWUSR, &shm_msg, &shm_msg_ptr);
    if(ret == cant_create_shm)
    {
        printf("can't create shared memory\n");
        exit(1);
    }
    if(ret == cant_attach_shm)
    {
        printf("can't attach shared memory\n");
        exit(1);
    }
    // create bcast shared memory
    ret = Shm_create(SHM_BCAST_FILE_NAME, SHM_BCAST_PROJ_ID, S_IRUSR | S_IWUSR, &shm_bcast, &shm_bcast_ptr);
    if(ret == cant_create_shm)
    {
        printf("can't create shared memory\n");
        exit(1);
    }
    if(ret == cant_attach_shm)
    {
        printf("can't attach shared memory\n");
        exit(1);
    }
    printf("shared memory created\n");
	// create login shared memory
    ret = Shm_create(SHM_LOGIN_FILE_NAME, SHM_LOGIN_PROJ_ID, S_IRUSR | S_IWUSR, &shm_login, &shm_login_ptr);
    if(ret == cant_create_shm)
    {
        printf("can't create shared memory\n");
        exit(1);
    }
    if(ret == cant_attach_shm)
    {
        printf("can't attach shared memory\n");
        exit(1);
    }
    printf("shared memory created\n");


    // create user list
	users_list = UsersList_create();
	struct Msg msg = {.data = "Null", .type = -1, .user_pid = -1};;

    pthread_t bcast_th;
    if (pthread_create(&bcast_th, NULL, (void *) bcast_thread, (void*) &msg) != 0) {
        perror("can't create bcast thread ");
        finalize(1);
    }
    if (pthread_detach(bcast_th) != 0) {
        perror("can't detach bcast thread ");
        finalize(1);
    }

	pthread_t login_th;
    if (pthread_create(&login_th, NULL, (void *) login_thread, (void*) &msg) != 0) {
        perror("can't create bcast thread ");
        finalize(1);
    }
    if (pthread_detach(login_th) != 0) {
        perror("can't detach bcast thread ");
        finalize(1);
    }


	while(1)
	{
		// sleep until the client sends a message
		recv_msg(msg_sem, shm_msg_ptr, &msg);
        if(msg.type == Login_msg) // login
        {
            printf("[server main] got Login_msg '%s' (%d)\n", msg.data, msg.user_pid);
            // unblock bcast_sem. thread will bcast this message to other users

            // add user to our user list
            ret =  UsersList_add(users_list, msg.user_pid, msg.data);
			if(ret == invalid_user_id)
			{
				printf("[server main] invalid user id\n");
				finalize(1);
			}
			if(ret == malloc_err)
			{
				printf("[server main] malloc err\n");
				finalize(1);
			}
			if(ret == user_exists)
			{
				printf("[server main] user exists\n");
				continue;
			}
            printf("[server main] added new user\n");
            UsersList_print(*users_list);

			Sem_unlock(local_sem, local_sem_WAKE_UP_BCAST);
            // bcast to new user our user list. user will find his name. connection proof
            // ??? how to do it??????  maybe new thread????? must be a new thread and new shm!!!!
			
			Sem_unlock(local_sem, local_sem_WAKE_UP_LOGIN);

        }
        else if(msg.type == Str_msg) // str msg
        {
            printf("[server main] got Str_msg '%s' (%d)\n", msg.data, msg.user_pid);
            // unblock bcast_sem. thread will bcast this message to other users
            Sem_unlock(local_sem, local_sem_WAKE_UP_BCAST);
        }
        else if(msg.type == Logout_msg)
        {
            printf("[server main] got Logout_msg '%s' (%d)\n", msg.data, msg.user_pid);

            // delete user from our user list
            ret = UsersList_delete_by_user_id(users_list, msg.user_pid);
            if(ret == NULL_ptr)
            {
                perror("[server main] cant delete user from user_list ");
                finalize(1);
            }
            printf("[server main] deleted user\n");
            UsersList_print(*users_list);
			// unblock bcast_sem. thread will bcast this message to other users
            Sem_unlock(local_sem, local_sem_WAKE_UP_BCAST);
        }
	}
}