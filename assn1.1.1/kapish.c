#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>

int init();
int loop();
int free_args(char **args[]);
int parse_cmd(char *cmd, char**args[]);
int exec(char * cmd, char* args[]);
void handle_sigint(int sig);

int cid;

int main(int argc, const char*argv[]){
//	for(int i = 0; i < argc; i++){
//		printf("%s\n", argv[i]);
//	}
//	return 0;

	
	init();
	signal(SIGINT, handle_sigint);
	char *args[512] = {""};
	exec("pwd", args);
	exec("ls", args);
	loop();
	return(0);
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
	return(0);
}

int loop(){
	char cmd[512] = {0};
	char *args[512] = {""};
	printf("? ");

	while (fgets(cmd, 512, stdin) != NULL){
		
		
		memset(args, 0, 512);	
		if (strcmp(cmd, "\n") == 0){
			printf("? ");
			continue;
		}
		//printf("cmd is |%s|\n", cmd);
		parse_cmd(cmd, &args);
		//printf("cmd is |%s|\n", cmd);
		if(strcmp(cmd, "setenv") == 0){
			//printf("setenv\n");
			char *val = "";
			if(args[2] != 0) val = args[2];
			setenv(args[1], val, 1);
			printf("? ");
			continue;
		} else if (strcmp(cmd, "unsetenv") ==0){
			//printf("unsetenv\n");
			unsetenv(args[1]);
			printf("? ");
			continue;
		}else if (strcmp(cmd, "cd") == 0){
			//printf("cd\n");
			char * dir = getenv("HOME");
			if (args[1] != 0) dir = args[1];
			chdir(dir);
			char *tmp[1] = {""};
			exec("pwd", tmp);
			printf("? ");
			continue;
		} else if (strcmp(cmd, "exit") == 0){
			printf("exiting\n");
			free_args(&args);
			return 0;
		}

		exec(cmd, args);
		printf("? ");
		free_args(&args);
	}
	return(0);
}



int free_args(char **args[]){
	int i = 0;
	for (; i < 512 && (args)[i] != 0; i++){
		free((args)[i]);
	}
	return(0);
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
	
	return(0);
}


int exec(char *cmd, char* args[]){
	
	cid = fork();

	//if(builtin){
	if(cid == 0){
		execvp(cmd, args);
	} else {
		wait();
	}
	cid = 0;
	return 0;

}

void handle_sigint(int sig){
	//printf("Handling sigint\n");
	if(cid != 0){ 
		printf("c: %d, mine: %d\n", cid, getpid());	
		kill(cid, SIGTERM);	
	}
	//printf("? ");
}


