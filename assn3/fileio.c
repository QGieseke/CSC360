#include <stdio.h>
#include <stdlib.h>
#include "diskcontroller.c"

int write_file(FILE*f);
int read_file(char* name, int num_blocks);


int main(){
	init();
	FILE * f = fopen("printsizes", "r");
	int num_blocks = write_file(f);
	read_file("printsizes_mk2", num_blocks);
	return 0;
}


int write_file(FILE* f){
	//char buff [BLOCKSIZE];
	int i;
	for(i = 0;;){
		char buff [BLOCKSIZE] = "";
		size_t t = fread(buff, BLOCKSIZE, 1, f);
		//if(t==0) break;
		printf("%s", buff);
		write_block(i, buff);
		i++;
		printf("t is: %d\n",(int) t);
		if(t==0) break;
	}
	printf("%d blocks written\n", i);
	return i;
}

int read_file(char * name, int num_blocks){
	FILE * f = fopen(name, "w");
	//char buff [BLOCKSIZE];
	int i;
	for(i = 0; i <= num_blocks; i++){
		char buff [BLOCKSIZE] = "";
		read_block(i, buff);
		fwrite(buff, BLOCKSIZE, 1, f);
	}
	printf("%d blocks read\n", i);
	fclose(f);
	return i;

}
