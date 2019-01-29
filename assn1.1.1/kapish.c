#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>

int main(int argc, const char*argv[]){
//	for(int i = 0; i < argc; i++){
//		printf("%s\n", argv[i]);
//	}
//	return 0;

	char rc_dir[128] = getenv("HOME");
	strcat(rc_dir, "/.kapishrc");
	printf("%s\n", rc_dir);
	FILE *rc = fopen("~/.kapishrc", 'r');

}


