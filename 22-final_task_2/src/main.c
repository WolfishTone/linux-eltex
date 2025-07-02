#include<include/CLI.h>
#include<include/driver.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>                                                                    
#include <sys/ipc.h>                                                                      
#include <fcntl.h>                                                                         
#include <sys/msg.h>                                                                       
#include <string.h>                                                                        
#include <unistd.h>  

#define PROJ_ID  11


int main()
{                                                               
    int fd = open(FILE_NAME, O_CREAT, S_IRWXU); // создаем файл, чтобы на него ссылатся   
    if (fd == -1)                                                                      
    {                                                                                  
        perror("  can't create or open file ");                      
        exit(1);                                                                   
    }                                                                                  
    close(fd);
    
    key_t key = 0;
    key = ftok(FILE_NAME, PROJ_ID);  
    int msg_q_id = msgget(key, IPC_EXCL | IPC_CREAT | S_IRUSR | S_IWUSR);                                                                                                      
    if(msg_q_id == -1)                                                                    
    {                                                                                  
        perror("  can't create msg queue ");                       
        exit(1);                                                                   
    }


    struct Args_str args_str;
	args_str.args = malloc(sizeof(char*)* INPUT_ARGS_NUM); 
	args_str.args[0] = malloc(sizeof(char)* INPUT_COMMAND_SIZE);

    struct drivers_arr drivers;
    drivers.ids = malloc(sizeof(pid_t)* BASE_DRIVERS_NUM);
    drivers.capacity = BASE_DRIVERS_NUM;
    drivers.size = 0;


    while(1)
    {
        printf(" <+>> ");
        int res = my_IO_get_command_n_operands(&args_str);
		if(res)
		{
			my_IO_print_color_msg("  get_command_n_operands return NULL ptr ", Red);
			exit(1);
		}
        CLI_process_command(args_str, msg_q_id, &drivers);




    }
}