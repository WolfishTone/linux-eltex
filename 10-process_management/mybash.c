#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define INPUT_COMMAND_SIZE 255
#define INPUT_ARGS_NUM 100



int main(int argc, char **argv)
{
	char * prompt_for_input;
	char** input_command = malloc(sizeof(char*)* INPUT_ARGS_NUM);
	input_command[0] = malloc(sizeof(char)* INPUT_COMMAND_SIZE);
	
	
	prompt_for_input = (argc == 2)? argv[1]: " <+>> ";
	
	_Bool is_running = 1;
	while(is_running)
	{		
		printf("\n--%s ", prompt_for_input);
		
		// read command
		char c;
		int arg_ind= 0, in_arg_ind = 0;
		while(1)
		{
			c = fgetc(stdin);
			if(c == '\n' || c == '\0')
			{
				input_command[arg_ind][in_arg_ind]= '\0';
				arg_ind++;
				if(input_command[arg_ind])
				{
					free(input_command[arg_ind]);
					input_command[arg_ind] = NULL;
				}
				break;
			}
			if (c == ' ')
			{
				for(;input_command[arg_ind][in_arg_ind++] == ' ';);
				input_command[arg_ind][in_arg_ind]= '\0';
				arg_ind++;
				input_command[arg_ind] = malloc(sizeof(char)* INPUT_COMMAND_SIZE);
				in_arg_ind = 0;
			}
			else
				input_command[arg_ind][in_arg_ind++] = c;
		}
		
		if(arg_ind > 0)
		{
			if(strcmp(input_command[0], "exit")== 0)
			{
				printf("выход\n");
				for(int i = 0; i< INPUT_ARGS_NUM; i++)
					if(input_command[i])
						free(input_command[i]);
				free(input_command);
				exit(0);
			}
			
			// exec cpmmand
			pid_t child = fork();
			if(child > 0)
			{
				int status;
				wait(&status);
				printf("-----------------------------\n");
				printf("статус завершения комманды %d\n", WEXITSTATUS(status));
			}
			if(child == 0)
				if(execvp(input_command[0], input_command) == -1)
				{
					perror("ошибка execv\n");
					exit(1);
				}
			if(child < 0)
			{
				perror("не удалось породить процесс\n");
				exit(1);
			}
		}
		
	}
}
