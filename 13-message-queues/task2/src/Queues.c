#include <./Queues.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>


enum ReturnValues Queue_create(int* msqid, char * file_name, int proj_id, int read_write_flags)
{
	int fd = open(file_name, O_CREAT, S_IRWXU);
	if (fd == -1)
		return cant_create_file;
	
	close(fd);
	
	key_t key = ftok(file_name, proj_id); 
	*msqid = msgget(key, IPC_CREAT | IPC_EXCL | read_write_flags);
	if(*msqid == -1)
		return cant_create_queue;
	return success;
}

enum ReturnValues Queue_open(int* msqid, char * file_name, int proj_id, int read_write_flags) // for client
{	
	key_t key = ftok(file_name, proj_id); 
	*msqid = msgget(key, read_write_flags);
	if(*msqid == -1)
		return cant_open_queue;
	return success;
}

enum ReturnValues Queue_delete(int msqid) // for server & client
{
	if(msgctl(msqid, IPC_RMID, NULL))
		return cant_delete_queue;
	return success;
}

enum ReturnValues Queue_bcast_msg(struct UsersList* users_list, int q, struct Msg* msg)
{
	long msg_type = msg->mtype;
	struct User* tmp = users_list->head;
	while(tmp != NULL)
	{
		msg->mtype = msg_type + tmp->user_id;
		if((msgsnd(q, msg, MSG_SIZE , IPC_NOWAIT)) < 0)
			return msgsnd_error;
		tmp = tmp->next;
	}
	return success;
}

enum ReturnValues Queue_send_user_list(struct UsersList* users_list, int q, pid_t user_id)
{ //send user list to new user
	struct Msg msg;
	struct User* tmp = users_list->head;
	msg.mtype = Login_msg + user_id;
	while(tmp != NULL)
	{
		sprintf(msg.mdata, "%d %s", tmp->user_id, tmp->user_name);
		if((msgsnd(q, &msg, MSG_SIZE , IPC_NOWAIT)) < 0)
			return msgsnd_error;
		tmp = tmp->next;
	}
	sprintf(msg.mdata, "0"); //end msg
	if((msgsnd(q, &msg, MSG_SIZE , IPC_NOWAIT)) < 0)
		return msgsnd_error;
	return success;
}

enum ReturnValues Queue_get_user_list(struct UsersList* users_list, int q, pid_t user_id)
{
	struct Msg msg;
	long msg_type = Login_msg + user_id;
	
	while(1)
	{
		if((msgrcv(q, &msg, MSG_SIZE, msg_type, 0)) < 0)
			return msgsnd_error;
		printf("recv '%s'\n", msg.mdata);
		pid_t user_id_m = atoi(msg.mdata);
		printf("user_id_m %d\n", user_id_m);
	
		if(user_id_m == 0)
			break;

		int name_begin= 0;
		for(; msg.mdata[name_begin]!= ' '; name_begin++);
		for(; msg.mdata[name_begin]== ' '; name_begin++);
		enum ReturnValues ret = UsersList_add(users_list, user_id_m, msg.mdata+name_begin);
		if(ret != success)
			return ret;
	}
	return success;
}

// enum ReturnValues broadcast_UserListMessage(int msqid, struct UsersList user_list)
// {
// 	struct UsersListMessage msg;	
// 	msg->mtype = UserList_msg;
	
// 	struct User* tmp = user_list.head;
// 	int i = 0;
// 	for(;tmp != NULL;i++)
// 	{
// 		msg.mdata[i].user_id = tmp->user_id;
// 		strncpy(msg.mdata[i].user_name, tmp->user_name, USER_NAME_SIZE);
// 		tmp = tmp->next;
// 	}
// 	msg.mdata[i].user_id = 0; // NULLtermination
	
// 	// TO DO: send user_list_msg to all clients
// 	return success;
// }
