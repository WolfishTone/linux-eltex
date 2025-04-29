#pragma once

#include <./UsersList.h>


#define MSG_SEM_PROJ_ID 10
#define LOCAL_SEM_PROJ_ID 11
#define BCAST_SEM_PROJ_ID 12
#define LOGIN_SEM_PROJ_ID 13
#define LOCAL_CLI_SEM_PROJ_ID 14

#define SHM_MSG_PROJ_ID 15
#define SHM_BCAST_PROJ_ID 16
#define SHM_LOGIN_PROJ_ID 17

#define MSG_SEM_FILE_NAME "/tmp/sem1_tmp_file"
#define LOCAL_SEM_FILE_NAME "/tmp/sem2_tmp_file"
#define BCAST_SEM_FILE_NAME "/tmp/sem3_tmp_file"
#define LOGIN_SEM_FILE_NAME "/tmp/sem4_tmp_file"
#define LOCAL_CLI_SEM_FILE_NAME "/tmp/sem5_tmp_file"

#define SHM_MSG_FILE_NAME "/tmp/shm1_tmp_file"
#define SHM_BCAST_FILE_NAME "/tmp/shm2_tmp_file"
#define SHM_LOGIN_FILE_NAME "/tmp/shm3_tmp_file"

#define MSG_SIZE 255
#define USER_NAME_SIZE 25

enum MSg_types
{
	Str_msg,
	Login_msg,
	Logout_msg,
};

#define Sever_death -1


#define msg_sem_NUM 3 // num of semaphores in set
#define msg_sem_MEM 0
#define msg_sem_IS_SEND 1
#define msg_sem_BLOCK_CLI_SEND 2

#define local_sem_NUM 2 // num of semaphores in set
#define local_sem_WAKE_UP_BCAST 0
#define local_sem_WAKE_UP_LOGIN 1

#define local_cli_sem_NUM 1 // num of semaphores in set
#define local_cli_sem_IS_LOGIN 0


#define bcast_sem_NUM 3 // num of semaphores in set
#define bcast_sem_MEM 0
#define bcast_sem_COUNT 1
#define bcast_sem_BCAST_START 2

#define login_sem_NUM 3 // num of semaphores in set
#define login_sem_MEM 0
#define login_sem_LOGIN_START 1
#define login_sem_IS_RECV 2


// escape-коды для вывода легенды и ошибок
#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define PURPLE "\033[35m"
#define GREEN "\033[32m"
#define BLUE "\033[36m"
#define DEFAULT "\033[0m"


struct Msg
{
	char data[MSG_SIZE];
	long type; 
	long msg_id;
	pid_t user_pid;
};

double wtime();

void print_msg(struct Msg msg);

void Sem_lock(int sem_id, int sem_num);
void Sem_unlock(int sem_id, int sem_num);
void Sem_wait(int sem_id, int sem_num);

#define PRIVATE 1
#define NAMED 0

enum ReturnValues Sem_create(char *file_name, int nsems, _Bool is_private ,int proj_id, int flags, int *sem);
enum ReturnValues Sem_open(char *file_name, int nsems, int proj_id, int flags, int *sem);
enum ReturnValues Sem_init(int msg_sem, int local_sem, int bcast_sem, int login_sem);
enum ReturnValues Sem_init_cli(int local_sem);
enum ReturnValues Sem_delete (int sem);


// create shared memory
enum ReturnValues Shm_create(char *shm_name, int proj_id, int flags, int *shm, struct Msg **shm_ptr);
enum ReturnValues Shm_open(char *file_name, int proj_id, int flags, int *shm, struct Msg **shm_ptr);
enum ReturnValues  Shm_detach(struct Msg *shm_ptr);
enum ReturnValues  Shm_delete(int shm);

void unblock_bcast_sem(int sem_id);
enum ReturnValues bcast_msg (int msg_sem, int bcast_sem, struct Msg *shm_ptr, int user_num, struct Msg* msg);
enum ReturnValues get_bcast_msg (int bcast_sem, struct Msg *shm_ptr, struct Msg* msg, long last_msg_id);
enum ReturnValues send_msg(int sem_id, struct Msg *shm_ptr, struct Msg* msg);
enum ReturnValues recv_msg(int sem_id, struct Msg *shm_ptr, struct Msg* msg);

enum ReturnValues send_user_list(int login_sem, struct Msg* shm_ptr, struct UsersList *users_list);
enum ReturnValues recv_user_list(int login_sem, struct Msg* shm_ptr, struct UsersList *users_list);
