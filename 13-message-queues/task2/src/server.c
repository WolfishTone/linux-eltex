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

#include <./Queues.h>

int msg_q; // get user msgs. read only
int bcast_q; // broadcast user messages, 
struct UsersList *users_list = NULL; // list of online users

void finalize(int ret_code)
{
	// bcast ending message
	struct Msg msg;
    msg.mtype = Logout_msg;
    sprintf(msg.mdata,"end of server");
	if(Queue_bcast_msg(users_list, bcast_q, &msg) == msgsnd_error)
	{
		printf("msgrcv problem in Str_msg bcast\n");
		exit(1);
	}

	struct msqid_ds msg_stat;
	while(1) // waiting for logout msgs
	{
		if(msgctl(msg_q, IPC_STAT, &msg_stat) < 0)
		{
			perror("can't get msg_q stat struct\n");
			exit(1);
		}

		if(msg_stat.msg_qnum != 0 || msg_stat.msg_stime == 0)
			break;
		sleep(1); // to minimize cpu usage
	}

	while(1) // getting all msg to correct queue clossing
	{
		if (msgrcv(msg_q, &msg, MSG_SIZE, 1, IPC_NOWAIT) < 0) {
			if(errno == ENOMSG) // if msg queue is empty
				break;
		}
		sleep(1); // to minimize cpu usage
	}

	enum ReturnValues ret = Queue_delete(msg_q);
	if(ret == cant_delete_queue)
	{
		perror("can't delete msg_q\n");
		exit(1);
	}
	ret = Queue_delete(bcast_q);
	if(ret == cant_delete_queue)
	{
		perror("can't delete bcast_q\n");
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


int main()
{
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

	// create queues
	enum ReturnValues ret = Queue_create(&msg_q, "./msg_q_tmp_file", MSG_Q_PROJ_ID, S_IRUSR | S_IWUSR);	
	if(ret == cant_create_file)
	{
		printf("Can't create/open file\n");
		exit(1);
	}
	if(ret == cant_create_queue)
	{
		printf("Can't create msg_q\n");
		exit(1);
	}
	printf("msg_q correctly opened\n");

	ret = Queue_create(&bcast_q, "./msg_q_tmp_file", BCAST_Q_PROJ_ID, S_IRUSR | S_IWUSR);	
	if(ret == cant_create_queue)
	{
		printf("Can't create bcast_q\n");
		finalize(1);
	}

	users_list = UsersList_create();
	int clients_num = 0;
	struct Msg client_msg;
	printf("bcast_q correctly opened\n");

	while(1)
	{
		int msgrcv_ret = msgrcv(msg_q, &client_msg, MSG_SIZE, Login_msg, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting Login_msg\n");
				finalize(1);
			}
		}
		else
		{
			printf("got Login_msg '%s'\n", client_msg.mdata);
			int name_begin= 0;
			
			for(; client_msg.mdata[name_begin]!= ' '; name_begin++);
			for(; client_msg.mdata[name_begin]== ' '; name_begin++);
			
			// bcast new user to old ones
			ret = Queue_bcast_msg(users_list, bcast_q, &client_msg);
			if(ret == msgsnd_error)
			{
				printf("msgrcv problem in new user bcast\n");
				finalize(1);
			}

			pid_t user_id = atoi(client_msg.mdata);
			ret =  UsersList_add(users_list, user_id, client_msg.mdata + name_begin);
			if(ret == invalid_user_id)
			{
				printf("invalid user id\n");
				finalize(1);
			}

			UsersList_print(*users_list);

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

			// send user list to new user. it will get it own pid and it is show he is loged in
			ret = Queue_send_user_list(users_list, bcast_q, user_id);
			if(ret == msgsnd_error)
			{
				printf("msgrcv problem in new user bcast\n");
				finalize(1);
			}

			printf("clients_num = %d\n", users_list->num);
		}

		msgrcv_ret = msgrcv(msg_q, &client_msg, MSG_SIZE, Str_msg, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{

				perror("problem with getting Str_msg\n");
				finalize(1);
			}
		}
		else
		{
			printf("got Str_msg '%s'\n", client_msg.mdata);
			ret = Queue_bcast_msg(users_list, bcast_q, &client_msg);
			if(ret == msgsnd_error)
			{
				printf("msgrcv problem in Str_msg bcast\n");
				finalize(1);
			}
		}

		msgrcv_ret = msgrcv(msg_q, &client_msg, MSG_SIZE, Logout_msg, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting logout_msg\n");
				exit(1);
			}
		}
		else
		{
			printf("got Logout_msg '%s'\n", client_msg.mdata);
			
			ret = UsersList_delete_by_user_id(users_list, atoi(client_msg.mdata));
			if(ret == NULL_ptr)
			{
				perror("cant delete user from user_list\n");
				finalize(1);
			}

			ret = Queue_bcast_msg(users_list, bcast_q, &client_msg);
			if(ret == msgsnd_error)
			{
				printf("msgrcv problem in message Logout_msg\n");
				finalize(1);
			}
			printf("clients_num = %d\n", clients_num);
			// bcast user to delete it
		}
		
	}
}
