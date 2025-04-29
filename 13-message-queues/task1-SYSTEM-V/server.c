# include <sys/types.h>
# include <sys/ipc.h>
#include <fcntl.h>
# include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MTEXT_SIZE 10

struct msgbuf {
   long mtype;       /* message type, must be > 0 */
   char mtext[MTEXT_SIZE];    /* message data */
};

int main()
{ 
	char f_name[] = "only_for_fd";
	int proj_id = 10;
	
	int fd = open(f_name, O_CREAT, S_IRWXU); // создаем файл, чтобы на него ссылатся
	if (fd == -1)
	{
		perror("проблема при создании или открытии файла\n");
		exit(1);
	}
	close(fd);
	
	key_t key = ftok(f_name, proj_id); 
	
	int msqid = msgget(key, IPC_EXCL | IPC_CREAT | S_IRUSR | S_IWUSR);
	
	if(msqid == -1)
	{
		perror("проблема при создании очереди сообщений\n");
		exit(1);
	}
	
	struct msgbuf msg;
	msg.mtype = 1;
	
	sprintf(msg.mtext, "Hi!");

	if((msgsnd(msqid, &msg, MTEXT_SIZE , 0)) < 0){
		perror("проблема отправки сообщения\n");
		exit(1);
	}
	printf("send message: %s\n", msg.mtext);
	
			
	if (msgrcv(msqid, &msg, MTEXT_SIZE, 10, 0) < 0) {
		perror("проблема получения сообщения\n");
		exit(1);
	}

	printf("recv message: %s\n", msg.mtext);
	
	if(msgctl(msqid, IPC_RMID, NULL))
	{
		perror("проблема удалении очереди\n");
		exit(1);
	}  
	exit(0);
}
