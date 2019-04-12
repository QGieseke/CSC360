#include "diskcontroller.c"
#define NAMESIZE 31

struct inode{
	int size;		//size of file
	int flags;		//flags, dir = 1, normal file = 0
	short di_blocks[10];	//direct access blocks
	short I_block;		//single indirection block
	short DI_block;		//double indirection block
} inode;

struct dir{
	char inode_num[16];	//the number of the inode
	char filename[16][NAMESIZE];	//filename (16 strings (max length 31 char)
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

//used to get/claim/free blocks required. Deals with the free block vector only
short get_block();
short claim_block(short block_num);
short free_block(short block_num);

//Same, but for the free inode vector
char get_inode();
char claim_inode(char inode_num);
char free_inode(char inode_num);

//returns the inode at the given index
//returns DYNAMIC inode buffer, MUST FREE one level up
struct inode* 	read_inode(char inode_num);
//writes the given inode to the given inode index
char 			write_inode(struct inode* inode_write, char inode_num);

//given the path, returns the inode struct of the file (or dir), as well as the inode index in inode_index (for future writing/editing)
//returns DYNAMIC inode buffer, MUST FREE one level up (same one from read_inode)
struct inode* 	getfile_inode(char *path, char* inode_index);


//useful buffers and other things (I could be more efficient with fewer reads since these are buffered and this is single threaded, but hey I dont want to go through and think about it so better safe than sorry)
char* delim = '/';
char free_vector [BLOCKSIZE] = {0};
char block_buf[BLOCKSIZE] = {0};
struct dir* root;
struct superblock* superblock_buf;
struct inode* inode_buf;