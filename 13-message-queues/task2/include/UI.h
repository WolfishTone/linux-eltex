#pragma once

#include <ncurses.h>
#include <./UsersList.h>
#include <./Msg_history.h>

#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define PURPLE "\033[35m"
#define GREEN "\033[32m"
#define BLUE "\033[36m"
#define DEFAULT "\033[0m"

#define MIN_TERM_H 10
#define MIN_TERM_W 80

struct Win_St
{
  WINDOW *win;
  int width;
  int height;
  int x_pos;
  int y_pos;
};

int open_windows(struct Win_St *mes_hist_win_st, struct Win_St *user_list_win_st, struct Win_St *input_win_st);
void print_user_list_win(struct Win_St win_st, struct UsersList *user_list);
void print_mes_hist_win(struct Win_St win_st, struct Msg_history* msg_hist);
void print_input_win(struct Win_St win_st, char *input_str, int str_len, char* user_name);