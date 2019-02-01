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
		memset(args, 0, 512);	

		//printf("cmd is |%s|\n", cmd);
		parse_cmd(&cmd, &args);
		//printf("cmd is |%s|\n", cmd);
		if(strcmp(cmd, "setenv") == 0){
			printf("setenv\n");
		} else if (strcmp(cmd, "unsetenv") ==0){
			printf("unsetenv\n");
		}else if (strcmp(cmd, "cd") == 0){
			printf("cd\n");
		} else if (strcmp(cmd, "exit") == 0){
			printf("exiting\n");
			return 0;
		}

		exec(cmd, args);
		printf("? ");
		free_args(&args);
	}



}



int free_args(char **args[]){
	int i = 0;
	for (; i < 512 && (args)[i] != 0; i++){
		free((args)[i]);
	}

}


int parse_cmd(char * cmd, char** args[]){
	char buf_cmd[512] = {0};
	strncpy(buf_cmd, cmd, 512);
	//printf("Copied cmd is |%s|\n", buf_cmd);

	char *delim = " \n"; //space and newline
	//*cmd = strtok(buf_cmd, delim);
	char *tmp_cmd = strtok(buf_cmd, delim);
	strncpy(cmd, tmp_cmd, 512);
	args[0] = strncpy((char *) malloc (strlen(tmp_cmd) * sizeof(char)), tmp_cmd, strlen(tmp_cmd));
	//printf("replaced cmd is |%s|\n", cmd);
	int i = 1;
	char *token = strtok(NULL, delim);
	while (token != NULL) {
		//*args[i] = token;
	//	printf("token is |%s|\n", token);
		int len = strlen(token);
		char *cpy = (char *) malloc(len * sizeof(char));
		strncpy(cpy, token, len);
		//printf("Copied token is |%s|\n",cpy); 
		args[i] = cpy;
		//printf("Placed pointer in array |%p|\n", *args[i]);
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

