#include <stdio.h>

enum { ADD= '1', REMOVE, FIND, LIST, EXIT };



struct abonent
{
	char name[10];
	char second_name[10];
	char tel[10];
};


_Bool is_names_eq (const char* name1, const char* name2)
{
	while(1)
	{
		if(*name1 != *name2)
			return 1; // не равны
		if(*name1== '\0')
			return 0;
	
		name1++;
		name2++;
	}
}

_Bool str_copy(const char* str, char* copy)
{
	if(!str || !copy)
		return 1;
		
	for(int i = 0; str[i] != 0; i++)
		copy[i] = str[i];
	return 0;
}

int str_length(const char* str)
{
	int length = 0;
	for(int i = 0 ; str[i]!= 0; i++)
		length++;
	return length;
}

int count_abonents(struct abonent *abonents)
{
	int count = 0;
	for(int i = 0; i< 100; i++)
		if(abonents[i].name[0] != 0)
			count++;
	return count;
}

void clear_abonent(struct abonent *abonent_for_clear)
{
	for(int i = 0; i< 10; i++)
	{
		abonent_for_clear->name[i] = 0;
		abonent_for_clear->second_name[i] = 0;
		abonent_for_clear->tel[i] = 0;
	}
}

struct abonent* find_abonent(char *find_name, int *start_index,  struct abonent *abonents)
{
	for(; *start_index < 100 ; (*start_index)++)
		if(abonents[*start_index].name[0] != 0)
		{
			if(!is_names_eq( abonents[*start_index].name, find_name )) // сравнение функции 
				return abonents + (*start_index)++;
		}
			
	return NULL;
}

_Bool add_abonent(char *name, char *second_name, char *tel, struct abonent *abonents)
{
	if(count_abonents(abonents) == 100)
		return 1; // база переполнена
		
	for(int i = 0 ; i< 100 ; i++)
		if(abonents[i].name[0] == 0)
		{
			str_copy(name, abonents[i].name);
			str_copy(second_name, abonents[i].second_name);
			str_copy(tel, abonents[i].tel);
			break;
		}
		
	return 0;
}

int remove_abonent(char *name, struct abonent *abonents)
{
	if(count_abonents(abonents) == 0)
		return 2; // в базе нет контактов
		
	int start_index= 0;
	struct abonent* finded_abonent = find_abonent(name, &start_index, abonents);
	if(!finded_abonent)
		return 1; // абонента с таким именем не существует
		
	clear_abonent(finded_abonent);
	return 0;
}

void print_abonent(char* name, char* second_name, char* tel)
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
	for(int i = 0; i< 100; i++)
		if(abonents[i].name[0] == 0)
			print_abonent("---------", "---------",  "---------");
		else
			print_abonent(abonents[i].name, abonents[i].second_name, abonents[i].tel);
}


void print_actions()
{
	printf(" 1) Добавить абонента\n");
	printf(" 2) Удалить абонента\n");
	printf(" 3) Поиск абонентов по имени\n");
	printf(" 4) Вывод всех записей\n");
	printf(" 5) Выход\n");
	printf(" <+>> ");
}

char read_action()
{
	char action;
	scanf(" %c", &action);
	while (action< ADD || action> EXIT)
	{
		printf("недопустимый символ\n <+>> ");
		scanf(" %c", &action);
	}
	return action;
}


_Bool check_tel(const char *tel)
{
	for(int i = 0; i< 9;i++)
		if(tel[i] < '0' && tel[i] > '9')
			return 1; // недопустимый символ
	return 0;
}

_Bool check_name(const char *name)
{
	for(int i = 0; i< 9;i++)
		if( (name[i] >= 'A' && name[i] <= 'Z') || 
			(name[i] >= 'a' && name[i] <= 'z') )
			return 0;
	return 1; // недопустимый символ
}


int main()
{
	_Bool is_running= 1;
	struct abonent abonents[100];
	for(int i = 0; i< 100; i++)
		clear_abonent(abonents+i); 
								
	while(is_running)
	{
		print_actions();
		char action= read_action();
		
		switch(action)
		{
			case ADD:
				char name[10], second_name[10], tel[10];
				
				printf("введите имя. оно может содержать до 9-и букв\n <+>> ");
				scanf("%s", name); 
				name[9]= 0;
				if(check_name(name))
				{
					printf("имя может содержать только буквы\n");
					break;
				}
				
				printf("введите фамилию. оно может содержать до 9-и букв\n <+>> ");
				scanf("%s", second_name);
				second_name[9]= 0;
				if(check_name(second_name))
				{
					printf("фамилия может содержать только буквы\n");
					break;
				}
				
				printf("введите номер. он может содержать до 9-и цифр\n <+>> ");
				scanf("%s", tel);
				tel[9]= 0;
				if(check_tel(tel))
				{
					printf("телефон может содержать только цифры\n");
					break;
				}
				
				_Bool add_result = add_abonent(name, second_name, tel, abonents);
				if(add_result)
					printf("база переполнена, удалите невостребованных абонентов\n");
				break;
				
			case REMOVE:
				char remove_name[10];
				printf("введите имя абонента, которого вы хотите удалить. имя может содержать до 9-и букв.\n <+>> ");
				scanf("%s", remove_name);
				remove_name[9]= 0;
				if(check_name(remove_name))
				{
					printf("имя может содержать только буквы\n");
					break;
				}
				
				int remove_result = remove_abonent(remove_name, abonents);
				if(remove_result== 2)
					printf("удаление невозможно, база пуста\n");
				if(remove_result== 1)
					printf("удаление невозможно, нет абонентов с указанным именем\n");
				break;
				
			case FIND:
				char find_name[10];
				printf("введите имя для поиска. имя может содержать до 9-и букв.\n <+>> ");
				scanf("%s", find_name);
				find_name[9]= 0;
				if(check_name(find_name))
				{
					printf("имя может содержать только буквы\n");
					break;
				}
				int start_index=0;
				struct abonent* finded_abonent= NULL;
				while(start_index < 100)
				{
					finded_abonent= find_abonent(find_name, &start_index, abonents);
					if(finded_abonent)
						print_abonent(finded_abonent->name, finded_abonent->second_name, finded_abonent->tel);		
					else
						start_index++;
				}
				break;
				
			case LIST:
				list_abonent(abonents);
				break;
			case EXIT:
				is_running= 0;
		}
	}
	return 0;
}
