#include "file.h"


int mkdir(char *path);
int rmdir(char *path);

char get_inode();
char claim_inode(char inode_num);
char free_inode(char inode_num);

struct inode* read_inode(char inode_num);
char write_inode(struct inode* inode_write, char inode_num);

int write_file(char *path, FILE *f);
int read_file(char *path);

int init_fs();

char free_vector [BLOCKSIZE] = {0};

int main(){
	init_fs();
	return 0;
}

int init_fs(){
	//init disk
	init();
	//write superblock
	struct superblock super;
	super.magic_number = 42;
	super.num_blocks = 4096;
	super.inodes = 128;
	super.inode_free[0] =0b10000000;
	super.iroot = 0;
	write_block(0, (char*) &super);
	
	//init free block vector
	//char free_vector [BLOCKSIZE];
	free_vector[0] = 0b11111111;
	free_vector[1] = 0b11100000;
	write_block(1, (char*) &free_vector);

	//init root
	struct dir root;
	//int i = get_inode();
	write_block(10, (char*) &root);



	return 0;
}

int mkdir(char *path){
	resolve path (first assume every dir created in root)
	create directory
		get inode
		get data block(s)
		write data block(s)
		write inode
		update parent directory(root)
	
	struct dir curdir = (struct dir*) malloc(sizeof(struct dir));
	char dirInodeIndex = get_inode();

#	struct inode dirInode = read_inode(dirInodeIndex);
	
	short dirBlock = get_block(); 
	
	dirInode->size = BLOCKSIZE;
	dirInode->di_blocks[0] = dirBlock;
	
	write_block(dirBlock, (char *) curdir);
#	write_inode();
	
#	update parent
	

}

short get_block(){
	//update free vector
	read_block(1, free_vector);
	short i;
	for(i = 0; !(free_vector[i/8] >> i%8 & 1) || i < BLOCKSIZE; i++);
	if(i != BLOCKSIZE)return i;
	return -1;
}

short claim_block(short block_num){
	read_block(1, free_vector);
	//binary trickery, errors if bit is already set, someone else is using the inode
	if ((free_vector[block_num/8] >> (block_num%8)) &1)return -1;
	free_vector[block_num/8] = free_vector[block_num/8] | (1<<(block_num%8));
	//write through policy for simplicity
	write_block(1, free_vector);
	//cant really have undefined behaviour
	return 0;
}
//same as above
short free_block(short block_num){
	read_block(1, free_vector);
	//if bit is not set, error (cant free an unallocated inode)
	if(!(free_vector[block_num/8] >> (block_num%8)) &1)return -1;
	free_vector[block_num/8] = free_vector[block_num/8] & (~(1<<(block_num%8)));
	write_block(1, free_vector);
	return 0;
}

char get_inode(){
	read_block(0, superblock_buf);
	
	char i;
	for(i = 0;  !(superblock_buf->inode_free[i/8] >> i%8 & 1)|| i < 128; i++);
	if(i != 128) return i;
	return -1;
}

char claim_inode(char inode_num){
	read_block(0, superblock_buf);

	if((superblock_buf->inode_free[inode_num/8] >> (inode_num/8))&1) return -1;
	superblock_buf->inode_free[inode_num/8] = superblock_buf->inode_free[inode_num/8] | (1<<(inode_num%8));
	write_block(0, superblock_buf);
	return 0;
}

char free_inode(char inode_num){
	read_block(0, superblock_buf);

	if((!superblock_buf->inode_free[inode_num/8] >> (inode_num/8))&1) return -1;
	superblock_buf->inode_free[inode_num/8] = superblock_buf->inode_free[inode_num/8] & (~(1<<(inode_num%8)));
	write_block(0, superblock_buf);
	return 0;
}

struct inode* read_inode(char inode_num){
	read_block((inode_num/8) + 2, block_buf);
	struct inode* read_inode = (struct inode*) malloc(sizeof(struct inode));
	inode inode_vec[16] = (struct inode*) block_buf;
	read_inode = inode_vec[inode_num%16];	//pointer BS?
	return read_inode;
}

char write_inode(struct inode* inode_write, char inode_num){
	read_block((inode_num/8)+2, block_buf);
	inode inode_vec[16] = (struct inode*) block_buf;
	inode_vec[inode_num%16] = inode_write;
	write_block((inode_num/8)+2, (char *) inode_vec);
	return 0;
}
