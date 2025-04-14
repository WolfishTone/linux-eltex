#include <./UsersList.h>

#include <string.h>

#include <stdio.h>



struct User* User_create(pid_t user_id, char* user_name, struct User* next, enum ReturnValues *ret)
{
	if(user_id <= 0)
	{
		*ret = invalid_user_id;
		return NULL;
	}
	if(!user_name)
	{
		*ret = NULL_ptr;
		return NULL;
	}
	
	struct User* new_user = malloc (sizeof(struct User));
	if(!new_user)
	{
		*ret =  malloc_err;
		return NULL;
	}
	
	new_user->user_id = user_id;
	strncpy(new_user->user_name, user_name, USER_NAME_SIZE);
	new_user->next = next;
	
	*ret =  success;
	return new_user;
}

struct UsersList* UsersList_create()
{
	struct UsersList *new_user_list= malloc (sizeof(struct UsersList));
	new_user_list->head= NULL; 
	new_user_list->num = 0;
	return new_user_list;
	
}

_Bool is_User_in_UsersList(struct UsersList users_list, pid_t user_id, char* user_name)
{
	struct User* tmp = users_list.head;
	while(tmp != NULL)
	{
		if(user_id == tmp->user_id)
			return 1; // user exist
		if(strcmp(user_name, tmp->user_name) == 0)
			return 1; // user exist
		tmp = tmp->next;
	}
	
	return 0; // user doesnt exist
}

enum ReturnValues UsersList_find_user_by_id(struct UsersList users_list, pid_t user_id, char* find_user_name)
{
	if(!find_user_name)
		return NULL_ptr;
	struct User* tmp = users_list.head;
	while(tmp != NULL)
	{
		if(user_id == tmp->user_id)
		{
			strncpy(find_user_name, tmp->user_name, USER_NAME_SIZE);
			return success;
		}
		tmp = tmp->next;
	}
	return user_doesnt_exist; // user doesnt exist
}


enum ReturnValues UsersList_add(struct UsersList* users_list, pid_t user_id, char* user_name)
{
	if(!users_list)
		return NULL_ptr;	
	if(user_id <= 0)
		return invalid_user_id;
	if(!user_name)
		return NULL_ptr;
	
	if(is_User_in_UsersList(*users_list, user_id, user_name))
		return user_exists;
	
	enum ReturnValues ret;
	
	users_list->head = User_create(user_id, user_name, users_list->head, &ret);
	
	if(!users_list->head)
		return ret; // error
	
	users_list->num++;
	
	return ret;	
}

enum ReturnValues UsersList_delete_by_user_id(struct UsersList* users_list, pid_t user_id) // for logout
{
	if(!users_list)
		return NULL_ptr;
	
	struct User* tmp = users_list->head;
	struct User* father_tmp = NULL;
	while(tmp != NULL)
	{
		if(user_id == tmp->user_id)
			break;
		father_tmp = tmp;
		tmp = tmp->next;
	}
	
	if(father_tmp)
		father_tmp->next = tmp->next;
	else
		users_list->head = tmp->next;
	
	free(tmp);
	tmp = NULL;
	users_list->num--;
	return success;
}

void User_free(struct User* user_list_head)
{
	if(user_list_head->next)
		User_free(user_list_head->next);
	free(user_list_head);
}

void UsersList_free(struct UsersList* users_list)
{
	if(users_list->head)
		User_free(users_list->head);
	free(users_list);
	users_list = NULL;
}

void UsersList_print(struct UsersList users_list)
{
	printf("user num : %d\n", users_list.num);
	struct User* tmp = users_list.head;
	while(tmp != NULL)
	{
		printf("	(%p) '%s' %d ->(%p)\n", tmp, tmp->user_name, tmp->user_id, tmp->next);
		tmp = tmp->next;
	}
}


//~ int main()
//~ {
	//~ struct UsersList* users_list = UsersList_create();
	
	//~ UsersList_add(users_list, 10, "uwu");
	//~ UsersList_add(users_list, 11, "uwu1");
	//~ UsersList_add(users_list, 10, "uwu2");
	//~ UsersList_add(users_list, 12, "uwu1");
	//~ UsersList_add(users_list, 12, "uwu2");
	
	//~ printf("ok\n");
	//~ UsersList_print(*users_list);
	//~ printf("ok\n");
	
	//~ UsersList_delete_by_user_id(users_list, 10);
	//~ UsersList_print(*users_list);
	//~ UsersList_add(users_list, 10, "uwu3");
	//~ UsersList_print(*users_list);
	//~ UsersList_delete_by_user_id(users_list, 11);
	//~ UsersList_print(*users_list);
	//~ UsersList_delete_by_user_id(users_list, 10);
	//~ UsersList_print(*users_list);
	//~ UsersList_delete_by_user_id(users_list, 12);
	//~ UsersList_print(*users_list);
	//~ UsersList_add(users_list, 10, "uwu3");
	//~ UsersList_print(*users_list);
	//~ UsersList_free(users_list);
//~ }

