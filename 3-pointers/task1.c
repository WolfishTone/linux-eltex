#include <stdio.h>

#define bin_print(num, niters) for(int i= niters; i>=0;i--) printf("%d", (num>>i)&1); printf("\n");	

int main()
{
	int num = 1;
	int user_num;  
	printf("enter number <+>> ");
	int scanf_result= scanf ("%d", &user_num);
	if(!scanf_result)
	{
		printf("Entered not number.\n");
		return 1;
	}
	
	int niters= sizeof(num)*8-1;
	printf("niters= %d\n", niters);
	printf("current num = ");
	bin_print(num, niters);
	
	niters= sizeof(char)*8-1;
	printf("niters= %d\n", niters);
	printf("user num = ");
	bin_print(user_num, niters);
	
	char* num_ptr = (char*) &num;
	num_ptr+=2;
	*num_ptr = (char)user_num;
	
	niters= sizeof(num)*8-1;
	printf("niters= %d\n", niters);
	printf("changed num = ");
	bin_print(num, niters);
	
	return 0;
}
