#include <stdio.h>
#include <malloc.h>

int main()
{
	int N;
	printf("Enter N <+>> ");
	scanf("%d", &N);
	if(N<2)
	{
		printf("entered small or negative number. only positive available\n");
		return 1;
	}
	int *arr= malloc(sizeof(int)*N);
	if(arr== 0)
	{
		printf("alloc error\n");
		return 1;
	}
	
	for(int i= 0; i< N; i++)
		arr[i]= i+1;
	
	printf("\ndirect array   -> ");
	for(int i= 0; i< N; i++)
		printf("%d ", arr[i]);
	printf("\n");
	
	int tmp;
	for(int i= 0; i< N/2; i++)
	{
		tmp= arr[i];
		arr[i]= arr[N-i-1];
		arr[N-i-1]= tmp;
	}
	
	printf("reversed array -> ");
	for(int i= 0; i< N; i++)
		printf("%d ", arr[i]);
	printf("\n");
	
	free(arr);
	return 0;
}
