#include<include/my_IO.h>
#include<include/driver.h>

enum Commands_codes
{
	Command_exit,
	Command_create_driver,
    Command_send_task,
    Command_get_status,
    Command_get_drivers,
    Command_help
};

#define FILE_NAME "only_for_fd.t"

void CLI_process_command(struct Args_str Args_str, int msg_q_id, struct drivers_arr* drivers);