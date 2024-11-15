#include <stdio.h>

_Bool is_contain_substring (char* str, char* sub)
{
	while(1)
	{
		if(*str== '\0')
		{
			if(*sub== '\0')
				return 0;
			else
				return 1; // подстрока оказалась длиннее
		}
		if(*sub== '\0')
			return 0;
				
		if(*str != *sub)
			return 1; // не равны
	
		str++;
		sub++;
	}
}

int main()
{
	char string [255], substring[255];
	char* string_p= string;
	
	printf("enter string <+>> ");
	int scanf_result= scanf ("%s", string);
	if(!scanf_result)
	{
		printf("nothing entered\n");
		return 1;
	}
	
	printf("enter sub string <+>> ");
	scanf_result= scanf ("%s", substring);
	if(!scanf_result)
	{
		printf("nothing entered\n");
		return 1;
	}
	
	int i;
	for(i= 0; i < 255; i++, string_p++)
		if(!is_contain_substring (string_p, substring))
			break;

	if(i== 255)
		string_p = NULL;
	
	if(string_p == NULL)
		printf("the string does not contain a substring\n");
	else
		printf("%s\n", string_p);
			
	return 0;
}
