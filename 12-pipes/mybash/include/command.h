#pragma once

struct Command
{
	char** input_command;
	int next_arg_ind; // used num of arguments. command name arg too
	int args_num; // num of allocated args
};


void Command_init(struct Command *command);
void Command_deinit(struct Command command);

void Command_add_arg(char* arg, struct Command *command);

void Command_print(struct Command command);
void Command_debug_print(struct Command command);
