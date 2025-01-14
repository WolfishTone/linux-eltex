#pragma once

struct abonent
{
	struct abonent* prev;
	struct abonent* next;
	char name[10];
	char second_name[10];
	char tel[10];
};


int count_abonents(struct abonent *abonents);
struct abonent* find_abonent(char *find_name, struct abonent *abonents);

_Bool add_abonent(char *name, char *second_name, char *tel, struct abonent **abonents);
int remove_abonent(char *name, struct abonent *abonents);

void print_abonent(char* name, char* second_name, char* tel);
void list_abonent(struct abonent *abonents);

void free_abonents(struct abonent* abonents);
