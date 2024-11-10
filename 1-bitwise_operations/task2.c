#include <stdio.h>

int main ()
{
	int num;
	int scanf_result= scanf ("%d", &num);
	if(!scanf_result)
	{
		printf("Entered not number.\n");
		return 1;
	}
	if(num> 0)
	{
		printf("Entered incorrect number. Only negative numbers are available.\n");
		return 1;
	}
	int niters= sizeof(int)*8-1;
	printf("num1 = ");
	for(int i= niters; i>=0;i--)
		printf("%d", (num>>i)&1);
	printf("\n");
	return 0;
}
