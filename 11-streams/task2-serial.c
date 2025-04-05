#include <stdio.h> 
#include <stdlib.h>

#define ITER_NUM 180000000

int sum_n(int start, int iter_num)
{
	for(int i= 0; i< iter_num; i++)
		start++;
	return start;
}

int main()
{
	long a= 0;
	a = sum_n(a, ITER_NUM);
	printf("sum = %ld\n", a);
	exit(0);
}
