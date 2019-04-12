#include <stdio.h>
#include <stdlib.h>

#define vdisk "./vdisk"

#define MB 1048576
#define MB2 2097152
#define NUM_BLOCKS 4096
#define BLOCKSIZE 512
#define NUM_INODES 128

int init();
int write_block(short blocknum, char* block_buf);
char* read_block(short blocknum, char* block_buf);