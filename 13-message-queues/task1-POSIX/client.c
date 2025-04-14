#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	char mq_name[]= "/my_mq\0";
	
	mqd_t mq = mq_open(mq_name, O_RDWR);

	if(mq == -1)
	{
		perror("проблема при создании очереди сообщений\n");
		exit(1);
	}
	
	char buf[10];
	
	struct mq_attr attr;
	
	if (mq_getattr(mq, &attr) == -1)
	{
		perror("проблема при получении атрибутов очереди сообщений\n");
		exit(1);
	}
	
	if(mq_receive(mq, buf, attr.mq_msgsize, 0) == -1)
	{
		perror("проблема получения сообщения\n");
		exit(1);
	}
	
	printf("recv message: %s\n", buf);
	
	if(mq_send(mq, "Hello!\0", attr.mq_msgsize, 0))
	{
		perror("проблема отправки сообщения\n");
		exit(1);
	}
	printf("send message: Hello!\n");
	
	mq_close(mq);
}
