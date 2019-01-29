#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>


int main(){
	printf("start\n");
	//fork();
	//printf("two\n");
	int id = fork();
	if (id == 0){
		printf("Child\n");
		execlp("pwd", "");
	}
	 else {
		printf("parent\n");
	}
	return 0;
	
}
