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

enum Status_values { ST_FREE, ST_BUSY};

#define BUFLEN 255

struct thread_status
{
	int client_socket; // поле заполняемое главным потоком. при отсутствии клиента - -1. после обработки клиента - -1.
	pthread_mutex_t mut; // мьютекс, управляющий работой потока
	enum Status_values status; // состояние потока. поток меняет, главный поток - проверяет
	_Bool stop_flag; // для корректного завершения взаимодействия клиентов с потоками при завершении работы программы.
};

struct thread_pool_node
{
	struct thread_status* stat;
	pthread_t thread;
};

// для безопасного свобождения памяти пула потоков
struct thread_pool_node* thread_pool = NULL;
int thread_pool_size = 0;

void *thread(struct thread_status* stat)
{
	printf( "SERVER %ld: успешно создан\n", pthread_self());

	while(1)
	{
		pthread_mutex_lock(&(stat->mut));
		if(stat->stop_flag) // ловим стоп флаг
		{
			printf( "SERVER %ld: пойман stop_flag\n", pthread_self());
			break;
		}

		stat->status = ST_BUSY;
		int msgLength;
		char buf[BUFLEN];
		while( ( msgLength = recv( stat->client_socket, buf, BUFLEN, 0 ) ) > 0 ) // общение с клиентом
		{
			printf( "SERVER %ld: сообщение от клиента %d длиной %d: %s\n", pthread_self(),
				stat->client_socket, msgLength, buf);
			if(strcmp(buf, "get time\0"))
			{
				printf( "SERVER %ld: получен некорректный запрос от клиента %d\n", pthread_self(), stat->client_socket);
				continue;
			}

			// get current time
			time_t t = time(NULL);
			struct tm tm = *localtime(&t);
			sprintf(buf, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

			if( send (stat->client_socket, buf, strlen(buf), 0 ) < 0 )
			{
				perror( "Проблемы с пересылкой.\n");
				pthread_exit(NULL);
			}
			memset(buf, '\0', BUFLEN);
		}
		close(stat->client_socket);
		printf( "SERVER %ld: корректное завершение соединения с клиентом %d\n", pthread_self(), stat->client_socket);
		stat->client_socket = -1;
		stat->status = ST_FREE;
	}
	pthread_exit(NULL);
}

void create_thread_pool(int thread_num)
{
	thread_pool_size = thread_num;
	thread_pool  = malloc(sizeof(struct thread_pool_node)*thread_pool_size);

	for(int i = 0; i< thread_pool_size; i++)
	{
		struct thread_status* tmp = malloc(sizeof(struct thread_status));
		tmp->client_socket = -1;
		tmp->status = ST_FREE;
		tmp->stop_flag = 0;
		pthread_mutex_init(&(tmp->mut),0);
		pthread_mutex_lock(&(tmp->mut));

		thread_pool[i].stat = tmp;

		if (pthread_create(&(thread_pool[i].thread), NULL, (void *) thread, (struct thread_status*) tmp) <0){
			perror("Не удалось создать поток. \n");
			exit(1);
		}
	}
}

_Bool add_client_to_thread_pool(int client_sock)
{
	for(int i = 0; i< thread_pool_size; i++)
		if(thread_pool[i].stat->status == ST_FREE)
		{
			thread_pool[i].stat->client_socket = client_sock;
			pthread_mutex_unlock(&(thread_pool[i].stat->mut));
			return 0;
		}
	return 1; // нет свободных потоков
}

void finalize_thread_pool(int signo)
{
	for(int i = 0; i< thread_pool_size; i++) // разблокируем потоки
	{
		thread_pool[i].stat->stop_flag = 1;
		pthread_mutex_unlock(&(thread_pool[i].stat->mut));
		pthread_join(thread_pool[i].thread, NULL);
		free(thread_pool[i].stat);
	}
	free(thread_pool);
	printf("SERVER: Пересылка завершена. Счастливо оставаться.\n");
	exit(0);
}


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("ВВЕДИ количество_потоков порт\n");
		exit(1);
	}

	signal( SIGINT, finalize_thread_pool);

	int server_sock, client_sock;
	struct sockaddr_in server_sock_addr;


	if(( server_sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		perror("Сервер не может открыть сокет.\n");
		exit(1);
	}

	memset(&server_sock_addr, '\0', sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	server_sock_addr.sin_port = htons((in_port_t)atoi(argv[2]));
	if ( bind( server_sock, (struct sockaddr *)&server_sock_addr, sizeof(server_sock_addr) ) ) {
		perror("Связывание tcp_socket неудачно.\n");
		exit(1);
	}
	int length = sizeof( server_sock_addr ) ;
	if ( getsockname( server_sock, (struct sockaddr *)&server_sock_addr, (socklen_t *)&length ) ){
		perror("Вызов getsockname неудачен.\n");
		exit(1);
	}

	listen(server_sock, 2) ;

	printf("SERVER MAIN: tcp_socket готов: %s %d\n", inet_ntoa(server_sock_addr.sin_addr), ntohs(server_sock_addr.sin_port));


	// создать пул потоков
	create_thread_pool(atoi(argv[1]));

	for(;;)
	{
		if ( ( client_sock = accept( server_sock, 0, 0 ) ) < 0 ) {
			perror("Неверный socket для клиента. \n");
			exit(1);
		}

		while(add_client_to_thread_pool(client_sock))
			printf("SERVER MAIN: нет свободных потоков\n");


	}
}
