#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	struct mq_attr attr=
	{
		.mq_flags = 0,
		.mq_maxmsg = 1,
		.mq_msgsize = 10,
		.mq_curmsgs = 0,
	};
	
	char mq_name[]= "/my_mq\0";
	
	mqd_t mq = mq_open(mq_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

	if(mq == -1)
	{
		perror("проблема при создании очереди сообщений\n");
		exit(1);
	}
	
	
	if(mq_send(mq, "Hi!\0", 4, 1))
	{
		perror("проблема отправки сообщения\n");
		exit(1);
	}
	printf("send message: Hi!\n");
	
	char buf[10] = "Hi!\0";
	
	while(!strcmp(buf, "Hi!\0")) // cравниваем полученное сообщение с отправленным нами.
	{							 // из цикла мы не выйдем пока клиент не заберет наше
		if(mq_receive(mq, buf, attr.mq_msgsize, 0) == -1) // сообщение и не пришлет свое.
		{
			perror("проблема получения сообщения\n");
			exit(1);
		}
		if(mq_send(mq, "Hi!\0", 4, 1))
		{
			perror("проблема отправки сообщения\n");
			exit(1);
		}
	}
	
	printf("recv message: %s\n", buf);
	
	mq_close(mq);
	mq_unlink(mq_name);
	exit(0);
}
