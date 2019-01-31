#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>

int init();
int exec(char * cmd, char* args[]);
int loop();

int main(int argc, const char*argv[]){
//	for(int i = 0; i < argc; i++){
//		printf("%s\n", argv[i]);
//	}
//	return 0;

	
	init();

	char *args[512] = {""};
	exec("pwd", args);
	exec("ls", args);
	loop();

}

int init(){
	char rc_dir[128] = {0};
	char cmd[512] = {0};
	
	//printf("%s\n", getenv("HOME"));
 	
	strncat(rc_dir, getenv("HOME"), 128);
	//printf("%s\n", rc_dir);
	
	strcat(rc_dir, "/.kapishrc");
	//printf("%s\n", rc_dir);
	FILE *rc = fopen(rc_dir, "r");
	
	printf(".Kapishrc opened\n");
	
	
	while(fgets(cmd, 512, rc) != NULL){
		
		printf("%s", cmd);		//replace with exec later
	}
}

int loop(){
	char cmd[512] = {0};
	char *args[512] = {""};
	printf("? ");

	while (fgets(cmd, 512, stdin) != NULL){
		printf("cmd is |%s|\n", cmd);
		exec(cmd, args);
		printf("? ");
	}



}

int parse_cmd(char * cmd, char** args[]){
	char buf_cmd[512] = *cmd;
	delim = " \n"; //space and newline
	//*cmd = strtok(buf_cmd, delim);
	strncpy(*cmd, strtok(buf_cmd, delim), 512);
	int i = 0;
	token = strtok(NULL, delim);
	while (token != NULL) {
		//*args[i] = token;
		strncpy(args[i], token, 512);
		i++;
		token = strtok(NULL, delim);
	}
	

}


int exec(char *cmd, char* args[]){
	
	int id = fork();

	//if(builtin){
	if(id == 0){
		execvp(cmd, args);
	} else {
		wait();
	}
	return 0;

}

