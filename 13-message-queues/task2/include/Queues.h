#pragma once

#include <./UsersList.h>


#define MSG_Q_PROJ_ID 10
#define BCAST_Q_PROJ_ID 11

#define QUEUE_FILE_NAME "./msg_q_tmp_file"

#define MSG_SIZE 255
#define USER_NAME_SIZE 25

#define	Str_msg 100000  /* common message from client. 
		client msg template: "<user_pid> <msg>" mtype = Login_msg
		server msg template: "<user_pid> <msg>" mtype = Login_msg + user_pid*/

#define Login_msg 200000 /*request for login from client and server to bcast. 
		client msg template: "<user_pid> <cli_name>" mtype = Login_msg
		server msg template: "<user_pid> <cli_name>" mtype = Login_msg + user_pid*/
	
#define Logout_msg 300000/*request for logout from client and server to bcast. template:
		client msg template: "<user_pid>" mtype = Logout_msg
		server msg template: "<user_pid>" mtype = Logout_msg + user_pid*/


struct Msg
{
	long mtype; 
	char mdata[MSG_SIZE];
};
	

enum ReturnValues Queue_create(int* msqid, char * file_name, int proj_id, int read_write_flags);
enum ReturnValues Queue_open(int* msqid, char * file_name, int proj_id, int read_write_flags);
enum ReturnValues Queue_delete(int msqid);
enum ReturnValues Queue_bcast_msg(struct UsersList* users_list, int q, struct Msg* msg);
enum ReturnValues Queue_send_user_list(struct UsersList* user_list, int q, pid_t user_id);
enum ReturnValues Queue_get_user_list(struct UsersList* user_list, int q, pid_t user_id);