#include <dirent.h>
#include <getopt.h>
#include <linux/limits.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// escape-коды для вывода легенды и ошибок
#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define PURPLE "\033[35m"
#define GREEN "\033[32m"
#define BLUE "\033[36m"
#define DEFAULT "\033[0m"

// коды ошибок для их обработки
enum Return_Codes
{
  SUCCESS,
  OPT_HAS_NO_OPERAND,
  INVALID_OPERAND,
  INVALID_OPT,
  MALLOC_ERROR,
  INVALID_TERM_SIZE
};

// коды опций путей для их обработки
#define LEFT_PATH_OPT 0
#define RIGHT_PATH_OPT 1

// минимальный размер терминала
#define MIN_TERM_H 15
#define MIN_TERM_W 70

// коды пар цветов
enum Color_pairs_indexes
{
  STDSCR_COLOR_PAIR = 1,
  WINDOWS_COLOR_PAIR,
  LINE_COLOR_PAIR
};

// в ncurses нет дефайна для клавиши esc поэтому пришлось создать
#define KEY_ESCAPE 27

// структура окна файлового менеджера
struct Commander_window
{
  WINDOW *win;
  int win_w;
  int win_h;
  // индекс файла находящегося в заголовке окна
  int top_file_ind;
  // номер текущего inode
  __ino_t current_line_ino;
  char *path;
  _Bool is_active;
};

int
get_paths (const int argc, char **argv, char **left_path, char **right_path)
{
  char *left_dir_name = NULL;
  char *right_dir_name = NULL;

  int option_index = 0;
  static struct option long_options[] = { { "pl", 1, 0, LEFT_PATH_OPT },
                                          { "pr", 1, 0, RIGHT_PATH_OPT },
                                          { 0, 0, 0, 0 } };

  int opt = 0;
  do
    {
      opt = getopt_long_only (argc, argv, ":", long_options, &option_index);
      switch (opt)
        {
        case 0:
          left_dir_name = optarg;
          break;
        case 1:
          right_dir_name = optarg;
          break;
        case '?':
          return OPT_HAS_NO_OPERAND;
        default:
          if (opt != -1)
            return INVALID_OPT;
        }
    }
  while (opt != -1);

  *left_path = malloc (sizeof (char) * PATH_MAX);
  *right_path = malloc (sizeof (char) * PATH_MAX);
  if (!left_path || !right_path)
    return MALLOC_ERROR;

  if (left_dir_name == NULL || left_dir_name[0] == '-')
    getcwd (*left_path, PATH_MAX);
  else
    {
      char absolute_path[PATH_MAX];
      realpath (left_dir_name, absolute_path);
      strcpy (*left_path, absolute_path);
    }

  if (right_dir_name == NULL || left_dir_name[0] == '-')
    getcwd (*right_path, PATH_MAX);
  else
    {
      char absolute_path[PATH_MAX];
      realpath (right_dir_name, absolute_path);
      strcpy (*right_path, absolute_path);
    }
  return SUCCESS;
}

void
print_legend ()
{
  printf ("%sUsage:%s ncurses-commander [--pl path] [--pr path]\n", PURPLE,
          DEFAULT);
  printf ("You can specify paths for left and right windows by using --pl "
          "--pr options.\n");
  printf (
      "If you don't specify pathes ncurses-commander will use current path\n");
}

// расчет размеров окон исходя из размера терминала и открытие окон
int
open_windows (struct Commander_window *left_win_st,
              struct Commander_window *right_win_st)
{
  int term_h, term_w;
  getmaxyx (stdscr, term_h, term_w);

  if (term_h < MIN_TERM_H || term_w < MIN_TERM_W)
    return INVALID_TERM_SIZE;

  int wins_w = term_w / 2;
  int wins_h = term_h - 2;

  int left_win_x_pos = 0;
  int right_win_x_pos = term_w / 2;
  int wins_y_pos = 1;

  if (left_win_st->win)
    delwin (left_win_st->win);
  if (right_win_st->win)
    delwin (right_win_st->win);

  left_win_st->win = newwin (wins_h, wins_w, wins_y_pos, left_win_x_pos);
  right_win_st->win = newwin (wins_h, wins_w, wins_y_pos, right_win_x_pos);

  left_win_st->win_h = wins_h;
  left_win_st->win_w = wins_w;

  right_win_st->win_h = wins_h;
  right_win_st->win_w = wins_w;

  return SUCCESS;
}

// покраска базового окна и вывод заголовка программы
void
print_bg_win_and_title ()
{
  clear ();
  int term_w = getmaxx (stdscr);
  wbkgd (stdscr, COLOR_PAIR (STDSCR_COLOR_PAIR));
  move (0, term_w / 2 - 8);
  attron (A_BOLD);
  printw ("SunSet Commander");
  attroff (A_BOLD);
  refresh ();
}

void
print_window (struct Commander_window *win_st)
{
  wclear (win_st->win);
  wbkgd (win_st->win, COLOR_PAIR (WINDOWS_COLOR_PAIR));
  box (win_st->win, 0, 0);

  int path_len = strlen (win_st->path);
  int x_indent;
  wattron (win_st->win, A_BOLD);
  if (win_st->is_active)
    wattron (win_st->win, A_REVERSE);
  if (path_len <= win_st->win_w - 4)
    {
      x_indent = (win_st->win_w - path_len) / 2;
      wmove (win_st->win, 0, x_indent);
      wprintw (win_st->win, "%s", win_st->path);
    }
  else
    {
      x_indent = 2;
      wmove (win_st->win, 0, x_indent);
      wprintw (win_st->win, "...%s",
               win_st->path + (path_len - win_st->win_w + 7));
    }

  if (win_st->is_active)
    wattroff (win_st->win, A_REVERSE);

  wmove (win_st->win, 1, (win_st->win_w - 24) / 2);
  wprintw (win_st->win, "NAME");
  wattroff (win_st->win, A_BOLD);
}

void
print_lines (struct Commander_window *win_st)
{
  init_pair (LINE_COLOR_PAIR, COLOR_BLUE, COLOR_MAGENTA);

  struct dirent **files_list;
  int file_num = scandir (win_st->path, &files_list, NULL, alphasort);

  wattron (win_st->win, A_BOLD);
  for (int i = 0; i < win_st->win_h - 3 && i + win_st->top_file_ind < file_num;
       i++)
    {
      if (win_st->current_line_ino
              == files_list[i + win_st->top_file_ind]->d_ino
          && win_st->is_active)
        wattron (win_st->win, A_REVERSE);

      if (files_list[i + win_st->top_file_ind]->d_type == DT_DIR)
        wattron (win_st->win, COLOR_PAIR (LINE_COLOR_PAIR));

      wmove (win_st->win, 2 + i, 1);
      wprintw (win_st->win, "%s",
               files_list[i + win_st->top_file_ind]->d_name);

      int name_filler = win_st->win_w
                        - strlen (files_list[i + win_st->top_file_ind]->d_name)
                        - 23;
      for (int i = 0; i < name_filler; i++)
        wprintw (win_st->win, " ");

      if (files_list[i + win_st->top_file_ind]->d_type == DT_DIR)
        wattroff (win_st->win, COLOR_PAIR (LINE_COLOR_PAIR));

      if (win_st->current_line_ino
              == files_list[i + win_st->top_file_ind]->d_ino
          && win_st->is_active)
        wattroff (win_st->win, A_REVERSE);
    }
  wattroff (win_st->win, A_BOLD);
  free (files_list);
}

enum Moves
{
  MOVE_INIT,
  MOVE_UP,
  MOVE_DOWN
};

void
change_current_file_ino (struct Commander_window *win_st, int move)
{
  struct dirent **files_list;
  int file_num = scandir (win_st->path, &files_list, NULL, alphasort);

  if (move == MOVE_INIT)
    {
      win_st->current_line_ino = files_list[1]->d_ino;
      win_st->top_file_ind = 1;
      free (files_list);
      return;
    }

  for (int i = win_st->top_file_ind; i < file_num; i++)
    if (win_st->current_line_ino == files_list[i]->d_ino)
      {
        if (move == MOVE_UP && i != 1)
          {
            win_st->current_line_ino = files_list[i - 1]->d_ino;
            if (i == win_st->top_file_ind)
              win_st->top_file_ind--;
          }
        else if (move == MOVE_DOWN && i != file_num - 1)
          {
            win_st->current_line_ino = files_list[i + 1]->d_ino;
            if (i - win_st->top_file_ind >= win_st->win_h - 4)
              win_st->top_file_ind++;
          }
        free (files_list);
        return;
      }
  free (files_list);
}

void
enter_dir (struct Commander_window *win_st)
{
  if (win_st->is_active == 0)
    return;
  struct dirent **files_list;
  int file_num = scandir (win_st->path, &files_list, NULL, alphasort);

  for (int i = 1; i < file_num; i++)
    if (win_st->current_line_ino == files_list[i]->d_ino
        && files_list[i]->d_type == DT_DIR)
      {
        int j = 0;
        for (; win_st->path[j] != 0; j++)
          ;
        if (i == 1)
          {
            for (; j >= 0; j--)
              if (win_st->path[j] == '/')
                break;
            if (j != 0)
              win_st->path[j] = 0;
            else
              win_st->path[1] = 0;
          }
        else
          {
            int indent = 1;
            if (j != 1)
              win_st->path[j] = '/';
            if (j == 1)
              indent = 0;

            for (int k = 0;
                 files_list[i]->d_name[k] != '!' && j + k + 1 < PATH_MAX; k++)
              win_st->path[j + k + indent] = files_list[i]->d_name[k];
          }
        change_current_file_ino (win_st, MOVE_INIT);
        return;
      }
}

int
main (int argc, char **argv)
{
  struct Commander_window left_win, right_win;

  {
    // получение путей для левого и правого окна
    char *left_path;
    char *right_path;

    int get_paths_result = get_paths (argc, argv, &left_path, &right_path);
    switch (get_paths_result)
      {
      case OPT_HAS_NO_OPERAND:
        fprintf (stderr, "%sError%s: Option has no operand\n", RED, DEFAULT);
        print_legend ();
        exit (EXIT_FAILURE);
      case INVALID_OPT:
        fprintf (stderr, "%sError%s: Invalid option\n", RED, DEFAULT);
        print_legend ();
        exit (EXIT_FAILURE);

      case MALLOC_ERROR:
        fprintf (stderr, "%sError%s: malloc() error\n", RED, DEFAULT);
        exit (EXIT_FAILURE);
      }

    DIR *left_dir = opendir (left_path);
    DIR *right_dir = opendir (right_path);
    if (!left_dir)
      {
        fprintf (stderr, "%sError%s: directory '%s' doesn't exist\n", RED,
                 DEFAULT, left_path);
        exit (EXIT_FAILURE);
      }
    if (!right_dir)
      {
        fprintf (stderr, "%sError%s: directory '%s' doesn't exist\n", RED,
                 DEFAULT, right_path);
        exit (EXIT_FAILURE);
      }

    closedir (left_dir);
    closedir (right_dir);

    left_win.path = left_path;
    right_win.path = right_path;
  }

  initscr ();

  if (has_colors () == FALSE)
    {
      endwin ();
      fprintf (stderr, "Error: Your terminal does not support colors\n");
      exit (EXIT_FAILURE);
    }

  // ввод-вывод
  raw (); // полный контроль над вводом с клавиатуры
  noecho ();
  curs_set (0);          // отключение курсора
  int key;               // ввод с клавиатуры
  keypad (stdscr, TRUE); // включение стрелочек

  // создать цвета и пары цветов в функции ?????!!!
  start_color ();
  init_pair (STDSCR_COLOR_PAIR, COLOR_WHITE, COLOR_BLUE);
  init_pair (WINDOWS_COLOR_PAIR, COLOR_WHITE, COLOR_MAGENTA);

  // окна
  left_win.is_active = 1;
  right_win.is_active = 0;

  left_win.win = NULL;
  right_win.win = NULL;

  change_current_file_ino (&left_win, MOVE_INIT);
  change_current_file_ino (&right_win, MOVE_INIT);

  int open_wins_result = open_windows (&left_win, &right_win);

  if (open_wins_result == INVALID_TERM_SIZE)
    {
      endwin ();
      free (left_win.path);
      free (right_win.path);
      fprintf (stderr, "%sError%s: Terminal size to small\n\n\n", RED,
               DEFAULT);
      exit (EXIT_FAILURE);
    }

  print_bg_win_and_title ();
  refresh ();

  print_window (&left_win);
  print_window (&right_win);

  wrefresh (left_win.win);
  wrefresh (right_win.win);

  while (1)
    {
      print_lines (&left_win);
      print_lines (&right_win);
      wrefresh (left_win.win);
      wrefresh (right_win.win);

      key = getch ();

      switch (key)
        {
        case KEY_UP:
          change_current_file_ino (&left_win, MOVE_UP);
          change_current_file_ino (&right_win, MOVE_UP);
          break;
        case KEY_DOWN:
          change_current_file_ino (&left_win, MOVE_DOWN);
          change_current_file_ino (&right_win, MOVE_DOWN);
          break;
        case '\n':
          enter_dir (&left_win);
          enter_dir (&right_win);

          print_window (&left_win);
          print_window (&right_win);
          break;
        case '\t':
          left_win.is_active ^= 1;
          right_win.is_active ^= 1;

          print_window (&left_win);
          print_window (&right_win);
          break;
        case KEY_RESIZE:
          open_wins_result = open_windows (&left_win, &right_win);
          if (open_wins_result == INVALID_TERM_SIZE)
            {
              endwin ();
              free (left_win.path);
              free (right_win.path);

              fprintf (stderr, "%sError%s: Terminal size to small\n\n\n", RED,
                       DEFAULT);
              exit (EXIT_FAILURE);
            }
          print_bg_win_and_title ();
          refresh ();
        }
      if (key == 'q' || key == 'Q' || key == KEY_ESCAPE)
        break;
    }

  endwin ();
  free (left_win.path);
  free (right_win.path);

  exit (EXIT_SUCCESS);
}
