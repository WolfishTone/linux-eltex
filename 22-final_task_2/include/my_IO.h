enum Сolors
{
  Black = 30,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  Light_Gray,
  Gray = 90,
  Light_Red,
  Light_Green,
  Light_Yellow,
  Light_Blue,
  Light_Magenta,
  Light_Cyan,
  White
};

#define INPUT_COMMAND_SIZE 100
#define INPUT_ARGS_NUM 100

struct Args_str // line contains command and operands
{
	char **args;
	int args_num;
};


void my_IO_print_color_msg(char* msg, enum Сolors color);

int my_IO_get_command_n_operands(struct Args_str *args_str);
void my_IO_free_command_n_operands(struct Args_str Args_str);
void my_IO_print_command_n_operands(struct Args_str Args_str);