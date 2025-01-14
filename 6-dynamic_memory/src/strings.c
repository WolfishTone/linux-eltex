#include <stdio.h>


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

