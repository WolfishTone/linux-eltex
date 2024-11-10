#include <stdio.h>

int main ()
{
	int num1, num2;
	int scanf_result1= scanf ("%d", &num1);
	int scanf_result2= scanf ("%d", &num2);
	if(!scanf_result1 || !scanf_result2)
	{
		printf("Entered not number.\n");
		return 1;
	}
	
	if(num1< 0)
	{
		printf("Entered incorrect first number. Only positive numbers are available.\n");
		return 1;
	}
	int niters= sizeof(int)*8-1;
		
	printf("num1       = ");
	for(int i= niters; i>=0;i--)
		printf("%d", (num1>>i)&1);
	printf("\n");
	printf("num2       = ");
	for(int i= niters; i>=0;i--)
		printf("%d", (num2>>i)&1);
	printf("\n");
	
	num1= num1& 0xff00ffff;
	num2= num2& 0x00ff0000;
	int num_result= num1 | num2;
	
	printf("num_result = ");
	for(int i= niters; i>=0;i--)
		printf("%d", (num_result>>i)&1);
	printf("\n");
	
	return 0;
}
