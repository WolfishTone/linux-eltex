#include <stdio.h>
#include <include/actions.h>


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
	if(!scanf(" %c", &action))
		return SCANF_ERROR;
	while (action< ADD || action> EXIT)
	{
		printf("недопустимый символ\n <+>> ");
		if(!scanf(" %c", &action))
			return SCANF_ERROR;
	}
	return action;
}
