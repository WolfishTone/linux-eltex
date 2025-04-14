#include <./Msg_history.h>
#include <stdio.h>


struct Msg_history*  Msg_history_create()
{
    struct Msg_history *msg_hist = malloc (sizeof(struct Msg_history));
    msg_hist->head = NULL;
    msg_hist->tail = NULL;
    msg_hist->num = 0;
    return msg_hist;
}

enum ReturnValues Msg_history_add(struct Msg_history *msg_hist, char *user_name, char *msg)
{
    if(!msg_hist || !user_name || !msg)
		return NULL_ptr;
    
    struct UI_Msg* new_msg = malloc(sizeof(struct UI_Msg));
    sprintf(new_msg->msg, "%s: %s", user_name, msg);
    
    if(msg_hist->head)
        msg_hist->head->prev = new_msg;
    else
    msg_hist->tail = new_msg;
    
    new_msg->next = msg_hist->head;
    new_msg->prev = NULL;
    msg_hist->head = new_msg;
    
    msg_hist->num++;
    return success;
}

enum ReturnValues Msg_history_pop_back(struct Msg_history *msg_hist)
{
    if(!msg_hist)
        return NULL_ptr;
    struct UI_Msg* tmp = msg_hist->tail; 
    msg_hist->tail = msg_hist->tail->prev;
    free(tmp);
    msg_hist->tail->next = NULL;
    msg_hist->num--;
    return success;
}

void UI_Msg_free(struct UI_Msg* msg_hist_head)
{
	if(msg_hist_head->next)
        UI_Msg_free(msg_hist_head->next);
	free(msg_hist_head);
}

void Msg_history_free(struct Msg_history* msg_hist)
{
	if(msg_hist->head)
        UI_Msg_free(msg_hist->head);
	free(msg_hist);
	msg_hist = NULL;
}

void Msg_history_print(struct Msg_history msg_hist)
{
    printf("msg num : %d\n", msg_hist.num);
    printf("head (%p)<- [(%p) '%s'] ->(%p)\n", msg_hist.head->prev, msg_hist.head, msg_hist.head->msg, msg_hist.head->next);
    printf("tail (%p)<- [(%p) '%s'] ->(%p)\n", msg_hist.tail->prev, msg_hist.tail, msg_hist.tail->msg, msg_hist.tail->next);

	struct UI_Msg* tmp = msg_hist.head;
	while(tmp != NULL)
	{
		printf("	 (%p)<- [(%p) '%s'] ->(%p)\n", tmp->prev, tmp, tmp->msg, tmp->next);
		tmp = tmp->next;
	}
}

/*int main()
{
    struct Msg_history* msg_hist =  Msg_history_create();
    Msg_history_add(msg_hist, "uwu", "hello!");
    Msg_history_print(*msg_hist);
    
    Msg_history_add(msg_hist, "uwu1", "well hello!");
    Msg_history_print(*msg_hist);

    Msg_history_add(msg_hist, "uwu2", "h hello!");
    Msg_history_add(msg_hist, "uwu3", "hi!");
    Msg_history_add(msg_hist, "uwu4", "world hello!");
    Msg_history_add(msg_hist, "uwu5", "dfdf hello!");
    Msg_history_add(msg_hist, "uwu6", "666!");
    Msg_history_add(msg_hist, "uwu7", "fetidralization will comortrup!");
    Msg_history_add(msg_hist, "uwu8", "hello there!");
    Msg_history_add(msg_hist, "uwu9", "hello off!");
    Msg_history_add(msg_hist, "uwu10", "hello on!");
    Msg_history_print(*msg_hist);
    Msg_history_pop_back(msg_hist);
    Msg_history_pop_back(msg_hist);
    Msg_history_pop_back(msg_hist);
    Msg_history_print(*msg_hist);
    Msg_history_free(msg_hist);

}*/