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
	if(num< 0)
	{
		printf("Entered incorrect number. Only positive numbers are available.\n");
		return 1;
	}
	int niters= sizeof(int)*8-1;
	int unit_counter= 0;
	for(int i= niters; i>=0;i--)
		if((num>>i)&1)
			unit_counter++;
	printf("количество единиц в двоичном представлении числа= %d\n",
		unit_counter);
	return 0;
}
