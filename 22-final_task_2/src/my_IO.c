#include<stdio.h>
#include<include/my_IO.h>
#include <stdlib.h>

void my_IO_print_color_msg(char* msg, enum Сolors color)
{
	printf("\E[%dm%s\E[0m", color, msg);
}

int my_IO_get_command_n_operands(struct Args_str *args_str) // get command, operands and return op num
{
	if(args_str->args == 0)
		return -1;
		
	char c;
	
	args_str->args_num = 0;
	int in_arg_ind = 0;
	while(1)
	{
		c = fgetc(stdin);
		if(c == '\n' || c == '\0')
		{
			args_str->args[args_str->args_num][in_arg_ind]= '\0';
			if(in_arg_ind > 0)
				args_str->args_num++;
			break;
		}
		if (c == ' ')
		{
			for(;args_str->args[args_str->args_num][in_arg_ind++] == ' ';);
			args_str->args[args_str->args_num][in_arg_ind-1]= '\0';
			args_str->args_num++;
			args_str->args[args_str->args_num] = malloc(sizeof(char)* INPUT_COMMAND_SIZE);
			in_arg_ind = 0;
		}
		else
			args_str->args[args_str->args_num][in_arg_ind++] = c;
	}
	return 0;
}

void my_IO_free_command_n_operands(struct Args_str Args_str)
{
    for(int i = 0; i< Args_str.args_num; i++)
        free(Args_str.args[i]);
    free(Args_str.args);
}

void my_IO_print_command_n_operands(struct Args_str Args_str)
{
    printf("  args_num= %d [", Args_str.args_num);
    printf("%s", Args_str.args[0]);
    for(int i = 1; i< Args_str.args_num; i++)
    {
        printf(" %s", Args_str.args[i]);
    }
    printf("]\n");
}