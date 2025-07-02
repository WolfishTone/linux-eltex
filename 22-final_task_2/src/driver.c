#include <include/driver.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>


int driver(int msg_q_id)
{
    pid_t driver_id =  getpid();
    _Bool busy_flag = 0;
    struct timeval busy_time, select_time, now, end;
    busy_time.tv_sec = 0;
    busy_time.tv_usec = 0;

    end.tv_sec = 0;
    busy_time.tv_usec = 0;

    busy_time.tv_sec = 0;
    busy_time.tv_usec = 0;

    select_time.tv_sec = 1;
    select_time.tv_usec = 0;

    struct msgbuf msg;
    msg.mtype = TAXI_STATION_MTYPE;
    sprintf(msg.mtext, "hi %d", driver_id);
    if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0){
        perror("msgsnd problem");
        exit(1);
    }

    while(1)
    {
        if(busy_flag)
        {
            gettimeofday(&now, NULL);
            timersub(&end, &now, &busy_time);

            if(busy_time.tv_sec > 0)
            {
                if (select(0, NULL, NULL, NULL, &select_time) < 0)
                {
                    perror("  select error ");
                    exit(1);
                }
            }
            else
                busy_flag = 0;
        }

        if (msgrcv(msg_q_id, &msg, MTEXT_SIZE, driver_id, IPC_NOWAIT) < 0) // try to recieve message
        {
            if(errno == ENOMSG)
                continue; // no msgs

            perror("  msgrcv problem1 ");
            exit(1);
        }

        if(!strncmp(msg.mtext, "end", 3))
        {
            printf("%d drivers end\n", driver_id);
            msg.mtype = TAXI_STATION_MTYPE;
            sprintf(msg.mtext, "end");
            if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
            {
                perror("  msgsnd problem ");
                exit(1);
            }
            exit(0);
        }

        else if(!strncmp(msg.mtext, "stat", 4))
        {
            msg.mtype = TAXI_STATION_MTYPE;
            sprintf(msg.mtext, "%s", (busy_time.tv_sec > 0) ? "Busy" : "Available");
            if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
            {
                perror("  msgsnd problem ");
                exit(1);
            }
        }
        else if(!strncmp(msg.mtext, "task", 4))
        {
            if(busy_time.tv_sec > 0)
                sprintf(msg.mtext, "Busy %ld", busy_time.tv_sec);
            else
            {
                struct timeval tmp;
                tmp.tv_sec = atoi(msg.mtext+5);
                gettimeofday(&now, NULL);
                timeradd(&now, &tmp, &end);
                busy_flag = 1;

                sprintf(msg.mtext, "Available");
            }

            msg.mtype = TAXI_STATION_MTYPE;
            if((msgsnd(msg_q_id, &msg, MTEXT_SIZE , 0)) < 0)
            {
                perror("  msgsnd problem ");
                exit(1);
            }
        }
    }
    return 0;
}
