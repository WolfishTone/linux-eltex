#include <stdio.h>
#include <malloc.h>

#include <./include/strings.h>
#include <./include/abonents.h>


int count_abonents(struct abonent *abonents) // перепитсать
{
	int count = 0;
	struct abonent* tmp_abonent = abonents;
	while(tmp_abonent)
	{
		count++;
		tmp_abonent = tmp_abonent->next;
	}
	return count;
}

struct abonent* find_abonent(char *find_name, struct abonent *abonents) // перепитсать
{
	struct abonent* tmp_abonent = abonents;
	while(tmp_abonent)
	{
		if(!is_names_eq( tmp_abonent->name, find_name ))
			return tmp_abonent;
		tmp_abonent = tmp_abonent->next;
	}
			
	return NULL;
}

_Bool add_abonent(char *name, char *second_name, char *tel, struct abonent** abonents)
{
	struct abonent* new_abonent = malloc(sizeof(struct abonent));
	if(!new_abonent)
		return 1;
	new_abonent->prev= NULL;
	new_abonent->next = *abonents;
	if(*abonents)
		(*abonents)->prev = new_abonent;
	
	str_copy(name, new_abonent->name);
	str_copy(second_name, new_abonent->second_name);
	str_copy(tel, new_abonent->tel);
	
	*abonents = new_abonent;
	return 0;
}

int remove_abonent(char *name, struct abonent *abonents) // перепитсать
{
	if(count_abonents(abonents) == 0)
		return 2; // в базе нет контактов
		
	struct abonent* finded_abonent = find_abonent(name, abonents);
	if(!finded_abonent)
		return 1; // абонента с таким именем не существует
	struct abonent *prev_abonent = finded_abonent->prev;
	struct abonent*next_abonent = finded_abonent->next;
	free(finded_abonent);
	if(prev_abonent)
		prev_abonent->next = next_abonent;
	if(next_abonent)
		next_abonent->prev = prev_abonent;
	return 0;
}

void print_abonent(char* name, char* second_name, char* tel) // перепитсать
{
	int name_indent = 9 - str_length(name);
	int s_name_indent = 9 - str_length(second_name);
	int tel_indent = 9 - str_length(tel);
	
	char name_f[10], s_name_f[10], tel_f[10];
	for(int i = 0; i< name_indent;i++)
		name_f[i]= ' ';
	name_f[name_indent]= 0;
	for(int i = 0; i< s_name_indent;i++)
		s_name_f[i]= ' ';
	s_name_f[s_name_indent]= 0;
	for(int i = 0; i< tel_indent;i++)
		tel_f[i]= ' ';
	tel_f[tel_indent]= 0;
	
	printf(" %s%s | %s%s | %s%s\n", name_f, name, s_name_f, second_name, tel_f, tel);
}
void list_abonent(struct abonent *abonents)
{	
	struct abonent* tmp_abonent = abonents;
	while(tmp_abonent)
	{
		print_abonent(tmp_abonent->name, tmp_abonent->second_name, tmp_abonent->tel);
		tmp_abonent = tmp_abonent->next;
	}
}

void free_abonents(struct abonent* abonents)
{
	if(abonents->next)
		free_abonents(abonents->next);
	free(abonents);
}
