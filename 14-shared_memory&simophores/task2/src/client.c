#include <./Communication.h>
#include <./Msg_history.h>
#include <./UI.h>


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> // for getpid
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>


pid_t pid;
int msg_sem;   // (mem) - exclusive access to shm_msg
               // (is_send) - unlock server to get clients msgs 
               // (block_cli_send) - block clients send while server bcast msgs

int bcast_sem; // (mem) exclusive access to shm_bcast
               // (count) block bcast until all users have read the message
               // (bcast_start) make wake up client bcast thread and make it sleep after bcast 

int login_sem; // (mem) exclusive access to shm_login
               // (login_start) make wake up client login thread and make it sleep after getting user list
               // (is_recv) confirmation of msg is recieved
int local_sem; 
_Bool is_logined = 0;

int shm_msg, shm_bcast, shm_login;
struct Msg * shm_msg_ptr;
struct Msg *shm_bcast_ptr;
struct Msg *shm_login_ptr;

struct UsersList *users_list = NULL; // list of online users
struct Msg_history* msg_hist; // history of messages

void finalize(int ret_code)
{
	// bcast ending message
  struct Msg logout_msg = {.data= "Logout", .type= Logout_msg, .user_pid= pid};
  send_msg(msg_sem, shm_msg_ptr, &logout_msg);


  if(Sem_delete (local_sem) == cant_delete_sem)
  {
      perror("cant delete semaphore ");
      exit(1);
  }
	// detach shared_memory
  Shm_detach(shm_msg_ptr);
  Shm_detach(shm_bcast_ptr);
  Shm_detach(shm_login_ptr);
	
	UsersList_free(users_list);
  Msg_history_free(msg_hist);

  printf("goodbye!\n");
	exit(ret_code);
}

void handler()
{
  endwin ();
	finalize(0);
}

void* bcast_thread(void*) // sending bcast msgs
{
  printf("bcast server thread created\n");
	struct Msg msg_bcast;
	enum ReturnValues ret;

  long last_bcast_msg_id = 0;

  while(1)
  {
    Sem_wait(bcast_sem, bcast_sem_BCAST_START);

    // sleep while bcast_sem_bcast_start
    ret = get_bcast_msg (bcast_sem, shm_bcast_ptr, &msg_bcast, last_bcast_msg_id);
    if(ret == rcv_error)
    {
      perror("[bcast_thread] failed to recieve bcast msg ");
      continue;
    }
    // if got repeated msg
    if(last_bcast_msg_id == msg_bcast.msg_id)
      continue;

    // printf("%s[bcast_thread] got msg (%g)%s ", GREEN, wtime(), DEFAULT); print_msg(msg_bcast);
    
    if(msg_bcast.type == Str_msg)
    {
      char find_user_name[USER_NAME_SIZE];
      ret = UsersList_find_user_by_id(*users_list, msg_bcast.user_pid, find_user_name); 
      // add msg to msg list
      ret = Msg_history_add(msg_hist, find_user_name, msg_bcast.data);
      if(ret == NULL_ptr)
      {
        perror("[bcast_thread] can't add message to message list ");
        finalize(1);
      }
    }
    if(msg_bcast.type == Login_msg)
    {
      // add user
      ret =  UsersList_add(users_list, msg_bcast.user_pid, msg_bcast.data);
      if(ret == invalid_user_id)
      {
        printf("[bcast_thread] invalid user id\n");
        finalize(1);
      }
      if(ret == malloc_err)
      {
        printf("[bcast_thread] malloc err\n");
        finalize(1);
      }
      if(ret == user_exists)
      {
        printf("[bcast_thread] user exists --->>>>>> "); print_msg(msg_bcast); 
        finalize(1);
      }
      if(!is_logined)
      {
        is_logined= 1;
        Sem_unlock(local_sem, local_cli_sem_IS_LOGIN);
      }
    }
    if(msg_bcast.type == Logout_msg)
    {
      if(msg_bcast.user_pid == Sever_death)
      {
        endwin ();
        finalize(0);
      }
      // delete user
      ret = UsersList_delete_by_user_id(users_list, msg_bcast.user_pid);
      if(ret == NULL_ptr)
      {
        perror("[bcast_thread] cant delete user from user_list ");
        finalize(1);
      }
    }
    last_bcast_msg_id = msg_bcast.msg_id;
  }
}


int main(int argc, char** argv)
{
  printf("client pid = %d\n", getpid());
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

  // open semaphores
  enum ReturnValues ret = Sem_open(MSG_SEM_FILE_NAME, msg_sem_NUM, MSG_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &msg_sem);
  if(ret == cant_create_sem)
  {
    perror("can't open msg_sem ");
    exit(1);
  }
  ret = Sem_open(BCAST_SEM_FILE_NAME, bcast_sem_NUM, BCAST_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &bcast_sem);
  if(ret == cant_create_sem)
  {
    perror("can't open bcast_sem ");
    exit(1);
  }
  ret = Sem_open(LOGIN_SEM_FILE_NAME, login_sem_NUM, LOGIN_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &login_sem);
  if(ret == cant_create_sem)
  {
    perror("can't open bcast_sem ");
    exit(1);
  }
  ret = Sem_create(LOCAL_CLI_SEM_FILE_NAME, local_cli_sem_NUM, PRIVATE, LOCAL_CLI_SEM_PROJ_ID, S_IRUSR | S_IWUSR, &local_sem);
  if(ret == cant_create_sem)
  {
    perror("can't create local_sem ");
    exit(1);
  }
  if(Sem_init_cli(local_sem) == cant_set_sem_val)
  {
    perror("can't init local_sem ");
    exit(1);
  }


  // create shared memory
  ret = Shm_open(SHM_MSG_FILE_NAME, SHM_MSG_PROJ_ID, S_IRUSR | S_IWUSR, &shm_msg, &shm_msg_ptr);
  if(ret == cant_open_shm)
  {
    perror("can't open msg shared memory ");
    exit(1);
  }
  if(ret == cant_attach_shm)
  {
    perror("can't attach msg shared memory ");
    exit(1);
  }
  ret = Shm_open(SHM_BCAST_FILE_NAME, SHM_BCAST_PROJ_ID, S_IRUSR | S_IWUSR, &shm_bcast, &shm_bcast_ptr);
  if(ret == cant_open_shm)
  {
    perror("can't open bcast shared memory ");
    exit(1);
  }
  if(ret == cant_attach_shm)
  {
    perror("can't attach bcast shared memory ");
    exit(1);
  }
  ret = Shm_open(SHM_LOGIN_FILE_NAME, SHM_LOGIN_PROJ_ID, S_IRUSR | S_IWUSR, &shm_login, &shm_login_ptr);
  if(ret == cant_open_shm)
  {
    perror("can't open login shared memory ");
    exit(1);
  }
  if(ret == cant_attach_shm)
  {
    perror("can't attach login shared memory ");
    exit(1);
  }

  // user list
  users_list = UsersList_create();
  // history of messages
  msg_hist =  Msg_history_create();

  pthread_t thread;
  if (pthread_create(&thread, NULL, (void *) bcast_thread, (void*) NULL) != 0) {
    perror("can't create bcast thread ");
    finalize(1);
  }
  // if (pthread_detach(thread) != 0) {
  //   perror("can't detach bcast thread ");
  //   finalize(1);
  // }

  // cli_name checking, sending
  pid = getpid();
  struct Msg msg; 
  msg.msg_id = 0;
  msg.type = Login_msg;
  msg.user_pid = pid;
  char user_name[USER_NAME_SIZE];
  if(argc == 2) {    
    if(strlen(argv[1]) > USER_NAME_SIZE) {
      printf("Max cli_name size = %d\n", USER_NAME_SIZE);
      finalize(1);
    }
    sprintf(user_name, "%s", argv[1]);   
  }
  else // if user didn't get uname  
    sprintf(user_name, "user_%d", pid);
  
  sprintf(msg.data, "%s", user_name);
  
  // printf("sended [%ld]'%s' -> %d\n", msg.type, msg.data, msg.user_pid);

  if(send_msg(msg_sem, shm_msg_ptr, &msg) == snd_error)
  {
    perror("can't send Login_msg ");
    finalize(1);
  }


  Sem_wait(local_sem, local_cli_sem_IS_LOGIN);
  printf("successful login\n");

  ret = recv_user_list(login_sem, shm_login_ptr, users_list);
  if(ret == invalid_user_id)
  {
      printf("invalid user id\n");
      finalize(1);
  }
  if(ret == malloc_err)
  {
      printf("malloc err\n");
      finalize(1);
  }
  printf("successful got user list\n");

  if(users_list->num == 0)
  {
    printf("can't login\n");
    finalize(1);
  }

  msg.type = Str_msg;

  // user interface
  struct Win_St mes_hist_win_st, user_list_win_st, input_win_st;

  
  initscr ();

  noecho ();
  halfdelay(3);         
  curs_set (0);  
  int key;
  char input_msg[INPUT_MSG_SIZE];
  memset(input_msg, 0, INPUT_MSG_SIZE);
  int input_msg_ind = 0;

  int open_wins_result = open_windows (&mes_hist_win_st, &user_list_win_st, &input_win_st);

  if (open_wins_result == invalid_term_size)
  {
      endwin ();
      fprintf (stderr, "%sError%s: Terminal size to small\n\n\n", RED,
              DEFAULT);
      finalize(1);
  }

  refresh();

  while (1)
  {
    print_user_list_win(user_list_win_st, users_list);
    print_input_win(input_win_st, input_msg, input_msg_ind, user_name);
    print_mes_hist_win(mes_hist_win_st, msg_hist);

    key = getch ();

    if(((key >= '!' && key <= '~') || key == ' ') && input_msg_ind < INPUT_MSG_SIZE)
      input_msg[input_msg_ind++] = key;
    else if(key == 27) // Esc
    {
      //exit
      endwin();
      finalize(0);
    }
    else if(key == '\b') // backspace
    {
      input_msg_ind = input_msg_ind? input_msg_ind-1: 0;
      input_msg[input_msg_ind] = 0;
    }

    else if(key == '\n')
    {
      sprintf(msg.data, "%s", input_msg);
      
      if(send_msg(msg_sem, shm_msg_ptr, &msg) == snd_error)
      {
        perror("can't send Str_msg ");
        finalize(1);
      }
      memset(input_msg, 0, INPUT_MSG_SIZE);
      input_msg_ind = 0;
    }
  }
}