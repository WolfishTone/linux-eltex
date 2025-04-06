#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_ARG_SIZE 255
#define COMMAND_ARGS_NUM 5


struct Command
{
	char** input_command;
	int next_arg_ind; // used num of arguments. command name arg too
	int args_num; // num of allocated args
};


void Command_init(struct Command *command)
{
	if(!command)
	{
		printf("Command_init got NULL ptr\n");
		exit(1);
	}
	
	command->input_command = calloc(sizeof(char*), COMMAND_ARGS_NUM);
	command->args_num = COMMAND_ARGS_NUM;
	command->next_arg_ind = 0;
}

void Command_deinit(struct Command command)
{
	for(int i= 0; i< command.args_num; i++)
		free(command.input_command[i]);
	free(command.input_command);
}

void Command_add_arg(char* arg, struct Command *command)
{	
	if(!command)
	{
		printf("Command_add_arg got NULL ptr\n");
		exit(1);
	}
	
	if(command->next_arg_ind == command->args_num)
	{
		command->input_command = realloc(command->input_command, sizeof(char*) * (command->args_num+1));
		command->input_command[command->args_num] = NULL;
		command->args_num++;
	}
	
	if(!arg) // NULL termination for execvp
	{
		if(command->input_command[command->next_arg_ind])
			free(command->input_command[command->next_arg_ind]);
		command->input_command[command->next_arg_ind] = NULL;
	}
	else
	{
		if(!command->input_command[command->next_arg_ind])
			command->input_command[command->next_arg_ind] = malloc(sizeof(char)* COMMAND_ARG_SIZE);
		strcpy(command->input_command[command->next_arg_ind], arg);
	}
	
	command->next_arg_ind++;
}

void Command_print(struct Command command)
{
	printf("	args_num = %d ", command.next_arg_ind);
	for(int i = 0 ;command.input_command[i]!= 0;i++)
		printf("'%s' ", command.input_command[i]);
	printf("(NULL)\n");
}

void Command_debug_print(struct Command command)
{
	for(int i= 0; i< command.args_num; i++)
		printf("%d : %p = '%s'\n", i, command.input_command[i], command.input_command[i]? command.input_command[i]: "NULL");
	printf("\n");
}
