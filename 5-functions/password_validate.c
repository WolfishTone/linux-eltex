#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int IsPassOk(void)
{
	char Pass[12]= "zbcdefghiklm";
	gets(Pass);
	return 0 == strcmp(Pass, "test");
}

int main(void)
{
	int PwStatus;
	puts("Enter password:");
	PwStatus = IsPassOk();
	if (PwStatus == 0) 
	{
		printf("Bad password!\n");
		exit(1);
	} 
	else 
		printf("Access granted!\n"); // Строка для которой нужно выяснить адрес
	return 0;
}
