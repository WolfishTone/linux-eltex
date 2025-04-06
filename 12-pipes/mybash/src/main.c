#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <./include/command.h>
#include <./include/pipeline.h>


int main(int argc, char **argv)
{
	struct Pipeline pipeline;
	Pipeline_init(&pipeline);
	
	char f_name[] = "buf";
	int fd = open(f_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
	close(fd);
	
	char * prompt_for_input = (argc == 2)? argv[1]: " <+>> ";
	
	_Bool is_running = 1;
	
	char arg[255];
	
	while(is_running)
	{		
		printf("\n--%s ", prompt_for_input);
		
		// read command
		char c;
		int in_arg_ind = 0;
		Pipeline_carriage_return(&pipeline);
		while(1)
		{
			c = fgetc(stdin);
			if(c == '\n' || c == '\0')
			{
				// конец ввода
				if(in_arg_ind != 0)
				{
					arg[in_arg_ind] = '\0';
					Pipeline_add_arg_to_command(arg, &pipeline);
				}
				Pipeline_add_arg_to_command(NULL, &pipeline);
				break;
			}
			
			else if (c == ' ')
			{
				// новый аргумент
				if(in_arg_ind != 0)
				{
					arg[in_arg_ind] = '\0';
					in_arg_ind = 0;
					Pipeline_add_arg_to_command(arg, &pipeline);
				}
			}
			
			else if(c == '|')
			{
				if(in_arg_ind != 0)
				{
					arg[in_arg_ind] = '\0';
					in_arg_ind = 0;
					Pipeline_add_arg_to_command(arg, &pipeline);
				}
				Pipeline_add_arg_to_command(NULL, &pipeline); // end of command
			}
			else
				arg[in_arg_ind++] = c;
		}
		
		if(pipeline.current_command_ind > 0)
		{
			if(strcmp(pipeline.commands[0].input_command[0], "exit")== 0)
			{
				printf("выход\n");
				Pipeline_deinit(pipeline);
				unlink(f_name);
				exit(0);
			}
			
			// exec cpmmand			
			for(int i = 0; i < pipeline.current_command_ind; i++)
			{
				pid_t child = fork();
				if(child > 0)
				{
					int status;
					wait(&status);
					printf("------------------------------\n");
					printf("статус завершения комманды %s %d\n\n",pipeline.commands[i].input_command[0], WEXITSTATUS(status));
				}
				
				if(child == 0)
				{
					int old_stdout = dup(STDOUT_FILENO);
					int in_pipe, out_pipe;
					
					in_pipe = open(f_name, O_RDONLY);
					dup2(in_pipe, STDIN_FILENO);
					close(in_pipe);
					
					if(i != pipeline.current_command_ind - 1)
					{
						out_pipe = open(f_name, O_WRONLY);
						dup2(out_pipe, STDOUT_FILENO);
						close(out_pipe);
					}
						
					
					if(execvp(pipeline.commands[i].input_command[0], pipeline.commands[i].input_command) == -1)
					{
						dup2(old_stdout, STDOUT_FILENO);
						perror("ошибка execv\n");
						exit(1);
					}
					printf("in exec\n");
				}
				if(child < 0)
				{
					perror("не удалось породить процесс\n");
					exit(1);
				}
			}
		}
		
	}
}
