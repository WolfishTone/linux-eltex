#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#define BUFLEN 255

struct req_queue_node {
    struct sockaddr_in data;
    struct req_queue_node* next;
};

struct req_queue {
	pthread_mutex_t mut;
    struct req_queue_node* front;
    struct req_queue_node* rear;
};


struct req_queue* req_queue_create() {
    struct req_queue* q = malloc(sizeof(struct req_queue));
    pthread_mutex_init(&(q->mut),0);

    q->front = NULL;
    q->rear = NULL;
    return q;
}

_Bool req_queue_is_empty(struct req_queue* queue) {
    return queue->front == NULL;
}

void req_queue_add (struct req_queue* q, struct sockaddr_in data) {
    struct req_queue_node* newNode = malloc(sizeof(struct req_queue_node));
    newNode->data = data;
    newNode->next = NULL;

	pthread_mutex_lock(&(q->mut));

    if (req_queue_is_empty(q)) {
        q->front = newNode;
		q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    pthread_mutex_unlock(&(q->mut));
}

void req_queue_print(struct req_queue* q)
{
	struct req_queue_node*tmp = q->front;
	while (tmp)
	{
		printf("%s %d\n", inet_ntoa(tmp->data.sin_addr), ntohs(tmp->data.sin_port));
		tmp = tmp->next;
	}
	printf("end of print\n");
}

_Bool req_queue_extract(struct req_queue* q, struct sockaddr_in* data) {
	pthread_mutex_lock(&(q->mut));
    if (req_queue_is_empty(q))
    {
		pthread_mutex_unlock(&(q->mut));
        return 1; // очередь пуста
	}

    struct req_queue_node* temp = q->front;
    if(data)
		*data = temp->data;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    pthread_mutex_unlock(&(q->mut));

    return 0;
}

void req_queue_free(struct req_queue* q) {
    while (!req_queue_extract(q, NULL));
    free(q);
}

struct req_queue* req_q = NULL;

struct thread_status
{
	int stop_flag;
	struct req_queue* q;
	int sock;
};

struct thread
{
	pthread_t thread;
	struct thread_status* stat;
};

struct thread* thread_pool = NULL;
int thread_pool_size = 0;

void *thread(struct thread_status* stat)
{
	printf( "SERVER %ld: успешно создан\n", pthread_self());

	while(1)
	{
		struct sockaddr_in client_addr;
		while(req_queue_extract(stat->q, &client_addr))
		{
			if(stat->stop_flag) // ловим стоп флаг
			{
				printf( "SERVER %ld: пойман stop_flag\n", pthread_self());
				pthread_exit(NULL);
			}
		}

		printf("SERVER %ld: запрос извлечен\n", pthread_self());

		char buf[BUFLEN];

		// get current time
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		sprintf(buf, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

		if (sendto(stat->sock, &buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
		{
			perror("Проблемы с sendto . \n");
			break;
		}
	}
	pthread_exit(NULL);
}

void create_thread_pool(int sock, int thread_num)
{
	thread_pool_size = thread_num;
	thread_pool  = malloc(sizeof(struct thread)*thread_pool_size);
	for(int i = 0; i< thread_pool_size; i++)
	{
		struct thread_status* tmp = malloc(sizeof(struct thread_status));
		tmp->stop_flag = 0;
		tmp->q = req_q;
		tmp->sock = sock;
		thread_pool[i].stat = tmp;
		if (pthread_create(&(thread_pool[i].thread), NULL, (void *) thread, (struct thread*) tmp) <0)
		{
			perror("Не удалось создать поток. \n");
			exit(1);
		}
	}
}

void finalize_thread_pool(int signo)
{
	for(int i = 0; i< thread_pool_size; i++) // разблокируем потоки
	{
		thread_pool[i].stat->stop_flag = 1;
		pthread_join(thread_pool[i].thread, NULL);
		free(thread_pool[i].stat);
	}
	free(thread_pool);
	req_queue_free(req_q);
	printf("SERVER: Пересылка завершена. Счастливо оставаться.\n");
	exit(0);
}

int main(int argc, char** argv) {
	printf("pid = %d\n", getpid());
	if(argc < 3)
	{
		printf("ВВЕДИ количество_потоков порт\n");
		exit(1);
	}

	signal( SIGINT, finalize_thread_pool);

	int sock, length;
	struct sockaddr_in servAddr, clientAddr;

    if(( sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть socket.\n");
		exit(1);
	}
	memset(&servAddr, '\0', sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servAddr.sin_port = htons((in_port_t)atoi(argv[2]));
	if ( bind( sock, (struct sockaddr *)&servAddr, sizeof(servAddr) ) ) {
		perror("Связывание socket неудачно.\n");
		exit(1);
	}
	length = sizeof( servAddr ) ;
	if ( getsockname( sock, (struct sockaddr *)&servAddr, (socklen_t *)&length ) ){
		perror("Вызов getsockname неудачен.\n");
		exit(1);
	}
	printf("SERVER MAIN: socket готов: %s %d\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

	req_q = req_queue_create();
	create_thread_pool(sock, atoi(argv[1]));
	// пул потоков

	for(;;)
	{
		length = sizeof(clientAddr);

		char buf[BUFLEN];
		if (recvfrom(sock, &buf, BUFLEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&length) < 0) {
		  perror("Плохой socket клиента.");
		  exit(1);
		}
		if(strcmp(buf, "get time\0"))
			continue;
		printf("SERVER MAIN: поступил запрос\n");
		req_queue_add (req_q, clientAddr);
	}
}
