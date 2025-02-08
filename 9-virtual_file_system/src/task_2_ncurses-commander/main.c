//~ #include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>



#include <linux/limits.h>


#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define PURPLE "\033[35m"
#define GREEN "\033[32m"
#define BLUE "\033[36m"
#define DEFAULT "\033[0m"

enum Return_Codes {SUCCESS, OPT_HAS_NO_OPERAND, INVALID_OPERAND, INVALID_OPT, MALLOC_ERROR, INVALID_TERM_SIZE};

#define LEFT_PATH_OPT 0
#define RIGHT_PATH_OPT 1

#define MIN_TERMINAL_H 15
#define MIN_TERMINAL_W 70



int get_paths(const int argc, char** argv, char **left_path, char **right_path)
{	
	char *left_dir_name = NULL;
	char *right_dir_name = NULL;
	
	int option_index = 0;
	static struct option long_options[] = {
		{"pl", 1, 0, LEFT_PATH_OPT},
		{"pr", 1, 0, RIGHT_PATH_OPT},
		{0, 0, 0, 0}
	};
	
	int opt= 0;
	do
	{
		opt = getopt_long_only (argc, argv, ":", long_options, &option_index);
		printf("opt = %d\n", opt);
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
				if(opt != -1)
					return INVALID_OPT;	
        }
	} while (opt != -1);
	
	*left_path= malloc(sizeof(char)*PATH_MAX);
	*right_path= malloc(sizeof(char)*PATH_MAX);
	if(!left_path || !right_path)
		return MALLOC_ERROR;
	
	if(left_dir_name == NULL || left_dir_name[0] == '-')
		getcwd(*left_path, PATH_MAX);
	else
	{
		char absolute_path[PATH_MAX];
		realpath(left_dir_name, absolute_path);
		strcpy(*left_path, absolute_path);
	}
	
	if(right_dir_name == NULL || left_dir_name[0] == '-')
		getcwd(*right_path, PATH_MAX);
	else
	{
		char absolute_path[PATH_MAX];
		realpath(right_dir_name, absolute_path);
		strcpy(*right_path, absolute_path);
	}
	return SUCCESS;
}

void print_legend()
{
	printf("%sUsage:%s ncurses-commander [--pl path] [--pr path]\n", PURPLE, DEFAULT);
	printf("You can specify paths for left and right windows by using --pl --pr options.\n");
	printf("If you don't specify pathes ncurses-commander will use current path");
}

int check_term_size(int term_h, int term_w) // дергаем при запуске и при изменении размера окна
{
	if(term_h > MIN_TERM_H && term_w > MIN_TERM_W)
		return SUCCESS;
	elae
		return INVALID_TERM_SIZE;
}


int main(int argc, char** argv)
{
	char* left_path;
	char* right_path;
	int get_paths_result = get_paths(argc, argv, &left_path, &right_path);
	switch(get_paths_result)
	{
		case OPT_HAS_NO_OPERAND:
			printf("%sError%s: Option has no operand\n", RED, DEFAULT);
			print_legend();
			exit(EXIT_FAILURE);
		case INVALID_OPT:
			printf ("%sError%s: Invalid option\n", RED, DEFAULT);
			print_legend();
			exit(EXIT_FAILURE);
			
		case MALLOC_ERROR:
			printf ("%sError%s: malloc() error\n", RED, DEFAULT);
			exit(EXIT_FAILURE);
	}
	printf("left path %s\nright path %s\n", left_path, right_path);
	
	DIR *left_dir = opendir(left_path);
	DIR *right_dir = opendir(right_path);
	if(!left_dir)
	{
		printf ("%sError%s: directory '%s' doesn't exist\n", RED, DEFAULT, left_path);
		exit(EXIT_FAILURE);
	}
	if(!right_dir)
	{
		printf ("%sError%s: directory '%s' doesn't exist\n", RED, DEFAULT, right_path);
		exit(EXIT_FAILURE);
	}
	
	int term_h, term_w;
	initscr();
    getmaxyx(stdscr, term_h, term_w);
    
    int is_term_size_correct = check_term_size(term_h, term_w);
    if(is_term_size_correct == INVALID_TERM_SIZE)
    {
		printf ("%sError%s: Terminal size to small\n", RED, DEFAULT);
		exit(EXIT_FAILURE);
	}
	
	int wins_w = term_w / 2;
	int wins_h = term_h -3;
	
	int left_win_x_pos = 0;
	int right_win_x_pos = term_w / 2;
	int wins_y_pos = 1;
	
	_bool active_win = 0; // active window
	
	
	MEVENT event; // мышье событие
	int key; // ввод с клавиатуры
	
	do
	{
		key = getch();
		switch(key)
		{
			case KEY_UP:
			case KEY_DOWN:
			case KEY_ENTER:
			case KEY_MOUSE:
				// переходы по директориям в текущем окне
				break;
			case '\t':
				// переключение между окнами.
				active_win ^= 1;
				break;
			case KEY_RESIZE;
				int is_term_size_correct = check_term_size(term_h, term_w);
				if(is_term_size_correct == INVALID_TERM_SIZE)
				{
					printf ("%sError%s: Terminal size to small\n", RED, DEFAULT);
					exit(EXIT_FAILURE);
				}
	
				wins_w = term_w / 2;
				wins_h = term_h -3;
				
				left_win_x_pos = 0;
				right_win_x_pos = term_w / 2;
				wins_y_pos = 1;
		}
				 
			
			
		}
		// отрисовка активного окна
		// отрисовка пассивного окна
		// терминал и его связь с интерфейсом?
		// редактор?
	} while(key != 'q' || key != 'Q' || );
	
	
	
	
	closedir(left_dir);
	closedir(right_dir);
	free(left_path);
	free(right_path);
	
	exit(EXIT_SUCCESS);
}










