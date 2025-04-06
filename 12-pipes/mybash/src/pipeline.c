#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <./include/command.h>

#define PIPELINE_COMMANDS_NUM 2

struct Pipeline
{
	struct Command* commands;
	int commands_num;
	int current_command_ind;
};

void Pipeline_init(struct Pipeline* pipeline)
{
	pipeline->commands = malloc (sizeof(struct Command)* PIPELINE_COMMANDS_NUM);
	pipeline->commands_num = PIPELINE_COMMANDS_NUM;
	pipeline->current_command_ind = 0;
	for(int i= 0; i< PIPELINE_COMMANDS_NUM; i++)
		Command_init(&pipeline->commands[i]);
}

void Pipeline_deinit(struct Pipeline pipeline)
{
	for(int i = 0; i< pipeline.commands_num; i++)
		Command_deinit(pipeline.commands[i]);
	free(pipeline.commands);
}

void Pipeline_add_arg_to_command(char* arg, struct Pipeline* pipeline)
{
	if(!pipeline)
	{
		printf("Command_init got NULL ptr\n");
		exit(1);
	}
	if(pipeline->current_command_ind == pipeline->commands_num)
	{
		pipeline->commands = realloc(pipeline->commands, sizeof(struct Command) * (pipeline->commands_num+1));
		Command_init(&pipeline->commands[pipeline->commands_num]);
		pipeline->commands_num++;
	}
	
	Command_add_arg(arg, &pipeline->commands[pipeline->current_command_ind]);
	if(arg == NULL)
	{
		pipeline->current_command_ind++;
		if(pipeline->current_command_ind < pipeline->commands_num) // carriage return to 0
			pipeline->commands[pipeline->current_command_ind].next_arg_ind = 0; 
	}
}

void Pipeline_carriage_return(struct Pipeline* pipeline)
{
	pipeline->current_command_ind = 0;
	pipeline->commands[0].next_arg_ind = 0;
}

void Pipeline_print(struct Pipeline pipeline)
{
	for(int i= 0; i< pipeline.current_command_ind; i++)
	{
		printf("command #%d\n", i);
		Command_print(pipeline.commands[i]);
	}
}
