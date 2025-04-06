#pragma once


struct Pipeline
{
	struct Command* commands;
	int commands_num;
	int current_command_ind;
};

void Pipeline_init(struct Pipeline* pipeline);
void Pipeline_deinit(struct Pipeline pipeline);

void Pipeline_add_arg_to_command(char* arg, struct Pipeline* pipeline);
void Pipeline_carriage_return(struct Pipeline* pipeline);

void Pipeline_print(struct Pipeline pipeline);
