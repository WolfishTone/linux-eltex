// реализация команд
#include<include/CLI.h>
#include<stdio.h>
#include <stdlib.h>
#include <sys/msg.h> 
#include <string.h>
#include <unistd.h>


void CLI_print_command_usage(enum Commands_codes command)
 {
	switch(command)
	{
		case Command_exit:
            my_IO_print_color_msg(" Exits program\n", Magenta);
			my_IO_print_color_msg("   Usage", Green); printf(" : exit\n");;
			break;
        case Command_help:
            my_IO_print_color_msg(" Shows available commands\n", Magenta);
			my_IO_print_color_msg("   Usage", Green); printf(" : help\n");
			break;
         case Command_create_driver:
            my_IO_print_color_msg(" Creates driver\n", Magenta);
            my_IO_print_color_msg("   Usage", Green); printf(" : create_driver\n");
			break;
        case Command_send_task:
            my_IO_print_color_msg(" Sends time task to driver with id\n", Magenta);
            my_IO_print_color_msg("   Usage", Green); printf(" : send_task ");
			my_IO_print_color_msg("<pid> <task_timer>\n", Yellow);
            break;
        case Command_get_status:
            my_IO_print_color_msg(" Shows driver's status by id\n", Magenta);
            my_IO_print_color_msg("   Usage", Green); printf(" : get_status ");
			my_IO_print_color_msg("<pid>\n", Yellow);
            break;
         case Command_get_drivers:
            my_IO_print_color_msg(" Shows ids and statuses of all drivers\n", Magenta);
            my_IO_print_color_msg("   Usage", Green); printf(" : get_drivers \n");
			break;
	}
 }


void CLI_exit(struct Args_str Args_str, int msg_q_id, struct drivers_arr *drivers)
{
    my_IO_free_command_n_operands(Args_str);
    
    struct msgbuf msg;
    sprintf(msg.mtext, "end");

    for(int i = 0; i< drivers->size; i++)
    {
        msg.mtype = drivers->ids[i];
        if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
        {
            perror("msgsnd problem");
            exit(1);
        }
        if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, TAXI_STATION_MTYPE, 0) < 0) 
        {
            perror("  msgrcv problem ");
            exit(1);
        }

    }
    if(msgctl(msg_q_id, IPC_RMID, NULL))                                                  
    {                                                                                  
        perror("  can't close ");                                     
        exit(1);                                                                
    }
    unlink(FILE_NAME);

    my_IO_print_color_msg("  Goodbye!\n", Green);
    exit(0);
}

void CLI_create_driver(int msg_q_id, struct drivers_arr *drivers)
{
    pid_t driver_id = fork();
    if (driver_id == -1) 
    {
        my_IO_print_color_msg("  Driver didn't created.\n", Red);
        perror("");
        exit(1);
    } 
    else if (!driver_id)
    {
        driver(msg_q_id);
    } 
    else 
    {
        struct msgbuf msg;
        if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, TAXI_STATION_MTYPE, 0) < 0) {
            perror("  msgrcv problem ");
            exit(1);
        }

        if(strncmp(msg.mtext, "hi ", 3))
        {
            my_IO_print_color_msg("  Driver didn't created. Wrong msg\n", Red);
            exit(1);
        }

        if(driver_id != atoi(msg.mtext+3))
        {
            my_IO_print_color_msg(" Driver didn't created. Wrong driver id\n", Red);
            exit(1);
        }

        if(drivers->size == drivers->capacity -1)
        {
            drivers->capacity *= 2;
            drivers->ids =  realloc(drivers->ids, drivers->capacity);
        }
        drivers->ids[drivers->size++] = driver_id;
        char print[255];
        sprintf(print, "  Driver %d created\n", driver_id);
        my_IO_print_color_msg(print, Green);
    }
}

void CLI_send_task(int msg_q_id, struct drivers_arr *drivers, pid_t driver_id, int time)
{
    int i = 0;
    for(; i< drivers->size; i++)
    {
        if(driver_id == drivers->ids[i])
            break;
    }
    char print[512];
    if(i == drivers->size)
    {
        sprintf(print, "  There is no driver with id %d\n", driver_id);
        my_IO_print_color_msg(print, Red);
        return;
    }
    
    struct msgbuf msg;

    sprintf(msg.mtext, "task %d", time);
    msg.mtype = driver_id;
    if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
    {
        perror("msgsnd problem");
        exit(1);
    }

    if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, TAXI_STATION_MTYPE, 0) < 0) {
        perror("  msgrcv problem ");
        exit(1);
    }

    //'Available' if driver free and get task
    //'Busy <time>' if busy
    if(strncmp(msg.mtext, "Available", 9))
    {
        int busy_time = atoi(msg.mtext + 5);
        sprintf(print, "  Driver %d is busy for %d seconds\n", driver_id, busy_time);
        my_IO_print_color_msg(print, Red);
    }
    else
    {
        sprintf(print, "  Driver %d got task\n", driver_id);
        my_IO_print_color_msg(print, Green);
    }
}

void CLI_get_status(int msg_q_id, struct drivers_arr *drivers, pid_t driver_id)
{
    int i = 0;
    for(; i< drivers->size; i++)
    {
        if(driver_id == drivers->ids[i])
            break;
    }
    char print[512];
    if(i == drivers->size)
    {
        sprintf(print, "  There is no driver with id %d\n", driver_id);
        my_IO_print_color_msg(print, Red);
        return;
    }
    
    struct msgbuf msg;

    sprintf(msg.mtext, "stat");
    msg.mtype = driver_id;
    if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
    {
        perror("msgsnd problem");
        exit(1);
    }

    if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, TAXI_STATION_MTYPE, 0) < 0) {
        perror("  msgrcv problem ");
        exit(1);
    }
    sprintf(print, "  Driver %d has status %s\n", driver_id, msg.mtext);
    my_IO_print_color_msg(print, Green);
}

void CLI_get_drivers(int msg_q_id, struct drivers_arr *drivers)
{
    char print[512];
    for(int i = 0; i< drivers->size; i++)
    {
        struct msgbuf msg;
        sprintf(msg.mtext, "stat");
        msg.mtype = drivers->ids[i];
        if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
        {
            perror("msgsnd problem");
            exit(1);
        }

        if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, TAXI_STATION_MTYPE, 0) < 0) {
            perror("  msgrcv problem ");
            exit(1);
        }
        sprintf(print, "  Driver %d has status %s\n", drivers->ids[i], msg.mtext);
        my_IO_print_color_msg(print, Green);
    }
}

void CLI_process_command(struct Args_str Args_str, int msg_q_id, struct drivers_arr *drivers) // checking the correctness of the command and execute it
{
    if(strcmp(Args_str.args[0], "exit") == 0)
    {
        if(Args_str.args_num != 1)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.\n", Red);
            CLI_print_command_usage(Command_exit);
            return;
        }
        CLI_exit(Args_str, msg_q_id, drivers);
    }

    // create_driver
    else if(strcmp(Args_str.args[0], "create_driver") == 0)
    {
        if(Args_str.args_num != 1)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.\n", Red);
            CLI_print_command_usage(Command_create_driver);
            return;
        }
        CLI_create_driver(msg_q_id, drivers);
    }

    // send_task <pid> <task_timer>
    else if(strcmp(Args_str.args[0], "send_task") == 0)
    {
        if(Args_str.args_num != 3)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.\n", Red);
            CLI_print_command_usage(Command_send_task);
            return;
        }
        pid_t driver_id = atoi(Args_str.args[1]);
        if(driver_id == 0) // pid must be > 0
        {
            my_IO_print_color_msg("  Incorrect argument type. <pid> must be a number > 0.\n", Red);
            CLI_print_command_usage(Command_send_task); 
            return;
        }

        int time = atoi(Args_str.args[2]);
        if(time == 0) // pid must be > 0
        {
            my_IO_print_color_msg("  Incorrect argument type. <task_timer> must be a number > 0.\n", Red);
            CLI_print_command_usage(Command_send_task); 
            return;
        }

        CLI_send_task(msg_q_id, drivers, driver_id, time);
    }

    // get_status <pid>
    else if(strcmp(Args_str.args[0], "get_status") == 0)
    {
        if(Args_str.args_num != 2)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.\n", Red);
            CLI_print_command_usage(Command_get_status);
            return;
        }
        pid_t driver_id = atoi(Args_str.args[1]);
        if(driver_id == 0) // pid must be > 0
        {
            my_IO_print_color_msg("  Incorrect argument type. <pid> must be a number > 0.\n", Red);
            CLI_print_command_usage(Command_get_status); 
            return;
        }
        CLI_get_status(msg_q_id, drivers, driver_id);
    }

    // get_drivers
    else if(strcmp(Args_str.args[0], "get_drivers") == 0)
    {
        if(Args_str.args_num != 1)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.\n", Red);
            CLI_print_command_usage(Command_get_drivers);
            return;
        }
        CLI_get_drivers(msg_q_id, drivers);
    }

    // help
    else if(strcmp(Args_str.args[0], "help") == 0)
    {
        if(Args_str.args_num != 1)
        {
            my_IO_print_color_msg("  Incorrect number of arguments.", Red);
            CLI_print_command_usage(Command_help);
            return;
        }
        CLI_print_command_usage(Command_exit);
        CLI_print_command_usage(Command_help);
        CLI_print_command_usage(Command_create_driver);
        CLI_print_command_usage(Command_send_task);
        CLI_print_command_usage(Command_get_status);
        CLI_print_command_usage(Command_get_drivers);
        printf("\n");
    }
    else
        my_IO_print_color_msg("  Incorrect command. Use help command to see all available commands.\n", Red);
}