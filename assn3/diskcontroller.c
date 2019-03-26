#include <stdio.h>
#include <stdlib.h>

#define vdisk "./vdisk"

#define MB 1048576
#define MB2 2097152
#define NUM_BLOCKS 4096
#define BLOCKSIZE 512

int init();
int write_block(int blocknum, char* block_buf);
char* read_block(int blocknum, char* block_buf);


//char block_buf[512];

/*
int main(){
	
	printf("Starting test\n");
	init();
	printf("disk initialized\n");

	char block_buf[BLOCKSIZE] = "This is some data in a string as text. Hello World!\0";
	write_block(0, block_buf);
	write_block(1, block_buf);
	write_block(100, block_buf);

	printf("Blocks written\n");
	
	char new_block_buf[BLOCKSIZE] = "This is diffrent data now.\0";
	write_block(1, new_block_buf);
	printf("Block 1 overwritten\n");

	char read_block_buf[BLOCKSIZE];
	printf("reading blocks\n");
	printf("Block 0:%s\n", read_block(0, read_block_buf));
	printf("Block 1:%s\n", read_block(1, read_block_buf));
	printf("Block 100:%s\n", read_block(100, read_block_buf));
	return 0;
}
*/

int init(){
//	printf("creating file\n");
	FILE *f = fopen("./vdisk", "wb");
	
	
	//printf("VDisk created\n");

	//printf("writing data to front of disk");
	//fputs("This is some text, Hello World!", f);
	
	fseek(f, MB2 , SEEK_SET);
	//printf("Cursor at %ld \n", ftell(f));
	
	fputc(0, f);
	//printf("0 printed at end\n");
	
	
	fclose(f);
	//printf("reading block 1\n");
	//read_block(0);
	//printf("done reading block 1\n");
	return 0;
}

char* read_block (int blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r");
	fseek(f, blocknum * BLOCKSIZE, SEEK_SET);
	printf("reading at %ld\n", ftell(f));
	fread(block_buf, BLOCKSIZE, 1, f);
	fclose(f);
	//printf("data as string: %s", block_buf);
	return block_buf;

}

int write_block(int blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r+");

	fseek(f, blocknum * BLOCKSIZE ,SEEK_SET);
	printf("writing at %ld\n", ftell(f));
	fwrite(block_buf, BLOCKSIZE, 1, f);

	fclose(f);
	return 0;

}
