#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>

int init();
int loop();
int free_args(char **args);
int parse_cmd(char *cmd, char**args);
int exec(char * cmd, char** args);
void handle_sigint(int sig);

int cid;

int main(int argc, const char*argv[]){
	
	signal(SIGINT, handle_sigint);
	init();
	loop();
	return(0);
}

//Reads the .kapishrc file and execs all commands within, echoing what is done
int init(){
	char rc_dir[128] = {0};
	char cmd[512] = {0};
	
	//get .kapishrc filepath 	
	strncat(rc_dir, getenv("HOME"), 128);
	strcat(rc_dir, "/.kapishrc");
	
	FILE *rc = fopen(rc_dir, "r");
	if(rc == NULL){
		printf("No .kapishrc file found\n");
	} else {
		printf(".Kapishrc opened\n");
		char * args[512] = {""};
			
		while(fgets(cmd, 512, rc) != NULL){
			memset(args, 0, 512);
			printf("%s", cmd);
			if(strcmp(cmd, "\n") == 0) continue;
			parse_cmd(cmd, args);
			if(exec(cmd, args)){
				printf("Invalid cmd\n");
				exit(0);
			}
			free_args(args);
		}
	}
	return(0);
}

//Main program loop, will parse commands given and exec them if valid. Has 4 builtin commands: set/unset env, cd, and exit
int loop(){
	char cmd[512] = {0};
	char *args[512] = {""};
	printf("? ");

	while (fgets(cmd, 512, stdin) != NULL){		
		
		memset(args, 0, 512);	
		//override for empty lines to avoid passing null values to other functions
		if (strcmp(cmd, "\n") == 0){
			printf("? ");
			continue;
		}
		
		parse_cmd(cmd, args);
		
		if(strcmp(cmd, "setenv") == 0){
			//printf("setenv\n");
			char *val = "";
			if(args[2] != 0) val = args[2];
			setenv(args[1], val, 1);

		} else if (strcmp(cmd, "unsetenv") ==0){
			//printf("unsetenv\n");
			unsetenv(args[1]);

		}else if (strcmp(cmd, "cd") == 0){
			//printf("cd\n");
			char * dir = getenv("HOME");
			if (args[1] != 0) dir = args[1];
			//printf("%s\n", dir);
			chdir(dir);
			char *tmp[1] = {""};
			exec("pwd", tmp);

		} else if (strcmp(cmd, "exit") == 0){
			printf("exiting\n");
			free_args(args);
			return 0;

		} else {
			if(exec(cmd, args)){
				printf("invalid cmd\n");
				return 0;
			}
		}

		printf("? ");
		free_args(args);
	}
	return(0);
}



int free_args(char **args){
	int i = 0;
	for (; i < 512 && (args)[i] != 0; i++){
		memset(args[i], 0, strlen(args[i]));
		free((args)[i]);
		
	}
	return(0);
}


int parse_cmd(char * cmd, char** args){
	char buf_cmd[512] = {0};
	strncpy(buf_cmd, cmd, 512);
	//printf("Copied cmd is |%s|\n", buf_cmd);

	char *delim = " \n"; //space and newline
	//*cmd = strtok(buf_cmd, delim);
	char *tmp_cmd = strtok(buf_cmd, delim);
	strncpy(cmd, tmp_cmd, 512);
	char * arg1 = (char *) malloc (strlen(tmp_cmd) * sizeof(char));
	memset(arg1, 0, strlen(tmp_cmd) * sizeof(char));
	args[0] = strncpy(arg1 , tmp_cmd, strlen(tmp_cmd));
	//printf("replaced cmd is |%s|\n", cmd);
	
	int i = 1;
	char *token = strtok(NULL, delim);
	while (token != NULL) {
		//*args[i] = token;
		//printf("token is |%s|\n", token);
		int len = strlen(token) + 1;
		//printf("Token length is %d\n", len);
		char *cpy = (char *) malloc(len * sizeof(char));
		memset(cpy, 0, len * sizeof(char));
		strncpy(cpy, token, len);
		cpy[len] = '\0';
		//printf("Copied token is |%s|\n",cpy); 
		args[i] = cpy;
		//printf("Placed pointer in array |%p|\n", args[i]);
		i++;
		token = strtok(NULL, delim);
	}
	
	return(0);
}


int exec(char *cmd, char** args){
	
	cid = fork();
	
	//if(builtin){
	if(cid == 0){
		if(execvp(cmd, args)) return 1;
	} else {
		wait();
	}
	cid = 0;
	return 0;

}

void handle_sigint(int sig){
	//printf("\n");
	if(cid != 0){ 
		//printf("c: %d, mine: %d\n", cid, getpid());	
		kill(cid, SIGTERM);
		printf("\n");	
	}
}


