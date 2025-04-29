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
	
	key_t key = ftok(f_name, proj_id); 
	
	int msqid = msgget(key, IPC_EXCL | S_IRUSR | S_IWUSR);
	if(msqid == -1)
	{
		perror("проблема при открытии очереди сообщений\n");
		exit(1);
	}
	
	struct msgbuf msg;
	
	if (msgrcv(msqid, &msg, MTEXT_SIZE, 1, 0) < 0) {
		perror("проблема получения сообщения\n");
		exit(1);
	}

	printf("recv message: %s\n", msg.mtext);
	
	msg.mtype = 10;
	sprintf(msg.mtext, "Hello!");
	if((msgsnd(msqid, &msg, strlen(msg.mtext)+1	, IPC_NOWAIT)) < 0){
		perror("проблема отправки сообщения\n");
		exit(1);
	}
	printf("send message: %s\n", msg.mtext);
	 
	exit(0);
}
