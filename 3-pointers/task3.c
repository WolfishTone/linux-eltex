#include <stdio.h>

#define N 10


int main()
{
	int arr [N];
	int* arr_p= arr;
	for(int i = 0; i < N; i++, arr_p++)
		*arr_p= i+1;
	
	arr_p= arr;
	for(int i = 0; i < N; i++, arr_p++)
		printf("%d ", *arr_p);
		
	printf("\n");
	return 0;
}
