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
	int *arr= malloc(sizeof(int)*N*N);
	if(arr== 0)
	{
		printf("alloc error\n");
		return 1;
	}
	for(int y= 0; y< N; y++)
		for(int x= 0; x<N; x++)
			arr[N*y+x]= N*y+x+1;

	printf("\n");
	for(int y= 0; y< N; y++)
	{
		for(int x= 0; x<N; x++)
			printf("%3d ", arr[N*y+x]);
		printf("\n");
	}
	
	free(arr);
	return 0;
}
