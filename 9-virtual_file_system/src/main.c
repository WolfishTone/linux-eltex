#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define STRING_LENGTH 17

int main()
{
	int d = open("output.txt", O_RDWR | O_CREAT, S_IRWXU);
	if(d == -1)
	{
		printf("ошибка создания и открытия файла\n");
		return 1;
	}
	char *buf = "String from file\0";
	size_t write_size= write(d, buf, STRING_LENGTH); 
	
	if(write_size != STRING_LENGTH)
	{
		printf("ошибка записи строки в файл\n");
		return 1;
	}
	
	sync();  
	
	char str[STRING_LENGTH];
	
	for(int i= 2 ;i <= STRING_LENGTH; i++)
	{
		if(lseek(d, -i, SEEK_END) == -1)
		{
			printf("ошибка перемещения коретки\n");
			return 1;
		} 
		if(read(d, str+i-2, 1) != 1)
		{
			printf("ошибка чтения из файла\n");
			return 1;
		} 
	}
	str[STRING_LENGTH-1] = '\0';
	
	close(d);
	
	printf("<+>> %s\n", str);
	return 0;
}
