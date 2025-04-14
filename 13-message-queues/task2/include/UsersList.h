#pragma once

#include <./ReturnValues.h>

#include <stdlib.h>


#define USER_NAME_SIZE 25
#define MAX_USERS_NUM 255



struct User
{
	pid_t user_id;
	char user_name[USER_NAME_SIZE];
	struct User* next;
};

struct UsersList
{
	struct User* head;
	int num;
};


struct User* User_create(pid_t user_id, char* user_name, struct User* next, enum ReturnValues *ret);
void User_free(struct User* user_list_head);

struct UsersList* UsersList_create();
_Bool is_User_in_UsersList(struct UsersList users_list, pid_t user_id, char* user_name);
enum ReturnValues UsersList_add(struct UsersList* users_list, pid_t user_id, char* user_name);
enum ReturnValues UsersList_delete_by_user_id(struct UsersList* users_list, pid_t user_id); // for logout
void UsersList_free(struct UsersList* users_list);
void UsersList_print(struct UsersList users_list);
enum ReturnValues UsersList_find_user_by_id(struct UsersList users_list, pid_t user_id, char* find_user_name);
