#include <ncurses.h>
# include <sys/types.h>
# include <sys/ipc.h>
#include <fcntl.h>
# include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <./Queues.h>
#include <./UI.h>
#include <./Msg_history.h>

#define SERVER_END 10

int msg_q = -1; // send msgs. write only
int bcast_q = -1; // broadcast user messages, 
struct UsersList *users_list; // list of online users
struct Msg_history* msg_hist; // history of messages
pid_t pid;

void finalize(int ret_code)
{
  if(msg_q!= -1)
  {
    struct Msg msg;
    msg.mtype = Logout_msg;
    sprintf(msg.mdata,"%d", pid);
    msgsnd(msg_q, &msg, MSG_SIZE , IPC_NOWAIT);
  }

  UsersList_free(users_list);
  Msg_history_free(msg_hist);
	printf("goodbye!\n");
	exit(ret_code);
}

void handler()
{
 	finalize(0);
}


int main(int argc, char** argv)
{
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

  // queues connect
  enum ReturnValues ret = Queue_open(&msg_q, "./msg_q_tmp_file", MSG_Q_PROJ_ID, S_IRUSR);
	if(ret == cant_open_queue)
	{
		perror("Can't open msg_q\n");
		exit(1);
	}
  printf("msg_q correctly opened\n");

  ret = Queue_open(&bcast_q, "./msg_q_tmp_file", BCAST_Q_PROJ_ID, S_IRUSR);
	if(ret == cant_open_queue)
	{
		printf("Can't open bcast_q\n");
		finalize(1);
	}
  printf("bcast_q correctly opened\n");

  // cli_name checking, sending
  pid = getpid();
  char user_name[USER_NAME_SIZE];
  struct Msg msg; 
  msg.mtype = Login_msg;
  if(argc == 2) {    
    if(strlen(argv[1]) > USER_NAME_SIZE) {
      printf("Max cli_name size = %d\n", USER_NAME_SIZE);
      finalize(1);
    }
    sprintf(user_name, "%s", argv[1]);   
  }
  else // if user didn't get uname  
    sprintf(user_name, "user_%d", pid);
  
  sprintf(msg.mdata, "%d %s", pid, user_name);

  if((msgsnd(msg_q, &msg, MSG_SIZE , 0)) < 0){
    perror("problem sending Login_msg\n");
    finalize(1);
  }
  printf("user name sended\n");

  // get bcast msg from server with our name
  users_list = UsersList_create();
  ret = Queue_get_user_list(users_list, bcast_q, pid);
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
  if(ret == user_exists)
  {
    printf("user exists\n");
    finalize(1);
  }
  if(atoi(msg.mdata) != pid)
  {
    perror("problem with login: server sends no our pid\n");
		finalize(1);
  }

  UsersList_print(*users_list);

  if(!is_User_in_UsersList(*users_list, pid, user_name))
  {
    printf("problems with connection to server\n");
    finalize(1);
  }
  printf("correct connect! congratulations! '%s'\n", msg.mdata);

  // history of messages
  msg_hist =  Msg_history_create();
  
  // user interface
  struct Win_St mes_hist_win_st, user_list_win_st, input_win_st;

  
  initscr ();

  if (has_colors () == FALSE)
  {
    endwin ();
    fprintf (stderr, "Error: Your terminal does not support colors\n");
    exit (EXIT_FAILURE);
  }

  // raw (); 
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
      struct Msg msg1;
      input_msg_ind = 0;
      msg1.mtype = Str_msg;
      printf("%d %s\n", pid, input_msg);
      sprintf(msg1.mdata, "%d %s", pid, input_msg);
      
      if((msgsnd(msg_q, &msg1, MSG_SIZE , 0)) < 0){
        perror("problem sending Str_msg\n");
        finalize(1);
      }
      memset(input_msg, 0, INPUT_MSG_SIZE);
    }

    int msgrcv_ret = msgrcv(bcast_q, &msg, MSG_SIZE, Logout_msg+pid, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting Logout_msg\n");
				finalize(1);
			}
		}
		else
		{
      if(atoi(msg.mdata) == 0)
      {
        endwin();
        printf("server end\n");
        finalize(0);
      }
      ret = UsersList_delete_by_user_id(users_list, atoi(msg.mdata));
      if(ret == NULL_ptr)
      {
        perror("cant delete user from user_list\n");
        finalize(1);
      }
    }

    msgrcv_ret = msgrcv(bcast_q, &msg, MSG_SIZE, Login_msg+pid, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting Login_msg\n");
				finalize(1);
			}
		}
		else
		{
      int name_begin= 0;
      for(; msg.mdata[name_begin]!= ' '; name_begin++);
			for(; msg.mdata[name_begin]== ' '; name_begin++);
      
      pid_t user_id = atoi(msg.mdata);

      ret =  UsersList_add(users_list, user_id, msg.mdata + name_begin);
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
			if(ret == user_exists)
			{
				printf("user exists\n");
				finalize(1);
			}
    }

    msgrcv_ret = msgrcv(bcast_q, &msg, MSG_SIZE, Logout_msg+pid, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting Login_msg\n");
				finalize(1);
			}
		}
		else
		{
      ret = UsersList_delete_by_user_id(users_list, atoi(msg.mdata));
    }

    msgrcv_ret = msgrcv(bcast_q, &msg, MSG_SIZE, Str_msg+pid, IPC_NOWAIT);
		if (msgrcv_ret < 0)
		{
			if(errno != ENOMSG)
			{
				perror("problem with getting Login_msg\n");
				finalize(1);
			}
		}
		else
		{
      int name_begin= 0;
      for(; msg.mdata[name_begin]!= ' '; name_begin++);
			for(; msg.mdata[name_begin]== ' '; name_begin++);

      char find_user_name[USER_NAME_SIZE];
      ret = UsersList_find_user_by_id(*users_list, atoi(msg.mdata), find_user_name); 
      Msg_history_add(msg_hist, find_user_name, msg.mdata+name_begin);

    }
  }
}