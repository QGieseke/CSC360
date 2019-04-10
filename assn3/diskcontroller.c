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

int init(){
	FILE *f = fopen("./vdisk", "wb");
	fseek(f, MB2 , SEEK_SET);
	fputc(0, f);
	fclose(f);
	return 0;
}

char* read_block (int blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r");
	fseek(f, blocknum * BLOCKSIZE, SEEK_SET);
	fread(block_buf, BLOCKSIZE, 1, f);
	fclose(f);
	return block_buf;
}

int write_block(int blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r+");
	fseek(f, blocknum * BLOCKSIZE ,SEEK_SET);
	fwrite(block_buf, BLOCKSIZE, 1, f);
	fclose(f);
	return 0;
}
