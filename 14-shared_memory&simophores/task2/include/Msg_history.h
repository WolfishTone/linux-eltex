#pragma once
#include <./Communication.h>

#define INPUT_MSG_SIZE MSG_SIZE - 10 // for UI
#define UI_MSG_SIZE INPUT_MSG_SIZE + USER_NAME_SIZE + 2

struct UI_Msg
{
    char msg[MSG_SIZE]; // "<user_name>: '<msg>'"
    struct UI_Msg* next;
    struct UI_Msg* prev;
};

struct Msg_history // linked list of messages which will be print in UI 
{
    struct UI_Msg* head;
    struct UI_Msg* tail;
    int num;
};

struct Msg_history*  Msg_history_create();
enum ReturnValues Msg_history_add(struct Msg_history *msg_hist, char *user_name, char *msg);
enum ReturnValues Msg_history_pop_back(struct Msg_history *msg_hist);
void UI_Msg_free(struct UI_Msg* msg_hist_head);
void Msg_history_free(struct Msg_history* msg_hist);
void Msg_history_print(struct Msg_history msg_hist);
