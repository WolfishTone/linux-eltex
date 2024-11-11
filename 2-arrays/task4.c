#include <stdio.h>
#include <malloc.h>

#include <unistd.h>

enum Sides {RIGHT=0, DOWN, LEFT, UP};

enum Sides changeSide(enum Sides side)
{
	switch(side)
	{
		case RIGHT:
			side= DOWN;
			break;
		case DOWN:
			side= LEFT;
			break;
		case LEFT:
			side= UP;
			break;
		case UP:
			side= RIGHT;
			break;
	}
	return side;
}

void changeXY(enum Sides side, int *x, int *y)
{
	switch(side)
	{
		case RIGHT:
			(*x)++;
			break;
		case DOWN:
			(*y)++;
			break;
		case LEFT:
			(*x)--;
			break;
		case UP:
			(*y)--;
			break;
	}
}

void correctXY(enum Sides side, int *x, int *y)
{
	switch(side)
	{
		case RIGHT:
			(*y)++;
			(*x)++;
			break;
		case DOWN:
			(*x)--;
			(*y)++;
			break;
		case LEFT:
			(*y)--;
			(*x)--;
			break;
		case UP:
			(*x)++;
			(*y)--;
			break;
	}
}

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
	int *arr= calloc(N*N ,sizeof(int));
	if(arr== 0)
	{
		printf("alloc error\n");
		return 1;
	}
	
	enum Sides side= RIGHT;
	int x= 0, y= 0;
	int sideLength= N;
	int counter= 1;
	int flag= 1; // 1- decrese sideLength 0- nothing
	
	while(sideLength)
	{
		for(int j= 0; j< sideLength;j++)
		{
			arr[y*N+x]= counter;
			changeXY(side, &x, &y);
			counter++;
		}
		side= changeSide(side);
		correctXY(side, &x, &y);
							
		if(flag)
			sideLength--;
		flag^=1;
	}
	
	printf("\n\n"); 
	for(int y= 0; y< N; y++)
	{ 
		for(int x= 0; x<N; x++) 
			printf("%3d ", arr[N*y+x]);
	printf("\n"); 
	}
		
	free(arr);
	return 0;
}
