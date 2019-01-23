#include<stdio.h>
#include<stdlib.h>
#include<sys\types.h>
#include<unistd.h>


int main(){
	printf("one\n");
	fork();
	printf("two\n");
	return 0;
	
}