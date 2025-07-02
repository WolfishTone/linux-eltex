#include <sys/types.h>


#define TAXI_STATION_MTYPE 1
#define MTEXT_SIZE 255



struct msgbuf {
   long mtype;       /* message type, must be > 0 */
   char mtext[MTEXT_SIZE];    /* message data */
};

#define BASE_DRIVERS_NUM 100

// array of existing drivers
struct drivers_arr
{
    pid_t *ids;
    int size;
    int capacity;
};

int driver(int msg_q_id);