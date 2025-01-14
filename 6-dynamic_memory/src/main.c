#include <stdio.h>

#include <include/strings.h>
#include <include/abonents.h>
#include <include/actions.h>


int main()
{
	_Bool is_running= 1;
	
	struct abonent *abonents= NULL;
	//~ for(int i = 0; i< 100; i++)
		//~ clear_abonent(abonents+i); 
								
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
				
				_Bool add_result = add_abonent(name, second_name, tel, &abonents);
				if(add_result)
					printf("база переполнена, удалите невостребованных абонентов\n");
				else
				{
					printf("абонент добавлен\n");
					print_abonent(abonents->name, abonents->second_name, abonents->tel);
				}
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
				else
					printf("абонент '%s' удален\n", remove_name);
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
				struct abonent* finded_abonent= find_abonent(find_name, abonents);
				while(finded_abonent)
				{
					print_abonent(finded_abonent->name, finded_abonent->second_name, finded_abonent->tel);
					finded_abonent= find_abonent(find_name, finded_abonent->next);
				}
				break;
				
			case LIST:
				list_abonent(abonents);
				break;
			case EXIT:
				is_running= 0;
				break;
			case SCANF_ERROR:
				printf("ошибка считывания\n");
		}
	}
	if(abonents)
		free_abonents(abonents);
	return 0;
}
