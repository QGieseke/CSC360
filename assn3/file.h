#include <stdio.h>
#include <stdlib.h>
#include "diskcontroller.c"

struct inode{
	int size;		//size of file
	int flags;		//flags
	short di_blocks[10];	//bdirect access blocks
	short i_block;		//single indirection block
	short di_block;		//double indirection block
} inode;

struct dir{
	char inode_num[16];	//the number of the inode
	char filename[16][31];	//filename (16 strings (max length 31 char
} dir,directory;

struct superblock{
	int magic_number;	//????
	int num_blocks;		//number of blocks in the whole system
	int inodes;		//number of inodes in the whole system
	char inode_free[16];
	short iroot;
	char free_space[483];
} superblock,sblock;

struct indirection_block{
	short block[256];	//block addresses for arbitrary assignment (single or double indirection)
}indirection_block, iblock;
