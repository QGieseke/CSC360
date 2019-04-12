#include "filecontroller.h"

//more comments in header file
short get_block(){
	//free vector
	read_block(1, free_vector);
	short i;
	//loop through the binary field (cant do algorithmically better than linear search)
	//BLOCKSIZE just happens to equal the number of blocks (jank)
	for(i = 0; !(free_vector[i/8] >> i%8 & 1) || i < BLOCKSIZE; i++);
	if(i != BLOCKSIZE)return i;
	//if every block is used somehow, -1 returned
	return -1;
}

short claim_block(short block_num){
	read_block(1, free_vector);
	//binary trickery, errors if bit is already set, ie someone else has claimed the inode
	if ((free_vector[block_num/8] >> (block_num%8)) &1)return -1;
	// num/8 gives index, num % 8 gives offset into the char.
	free_vector[block_num/8] = free_vector[block_num/8] | (1<<(block_num%8));
	//write through policy for simplicity
	write_block(1, free_vector);
	//cant really have undefined behavior
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
	for(i = 0; !(superblock_buf->inode_free[i/8] >> i%8 & 1) || i < NUM_INODES; i++);
	if(i != NUM_INODES) return i;
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

//returns DYNAMIC inode buffer
struct inode* read_inode(char inode_num){
	read_block((inode_num/8) + 2, block_buf);
	struct inode* r_inode = (struct inode*) malloc(sizeof(struct inode));
	inode inode_vec[16] = (struct inode*) block_buf;	//cast block to inode array
	*r_inode = inode_vec[inode_num%16];		//return inode at wanted index
	return r_inode;
}

char write_inode(struct inode* inode_write, char inode_num){
	read_block((inode_num/8)+2, block_buf);
	inode inode_vec[16] = (struct inode*) block_buf;
	inode_vec[inode_num%16] = inode_write;
	write_block((inode_num/8)+2, (char *) inode_vec);
	return 0;
}

struct inode* getfile_inode(char *path, char* inode_index){
	//iterate through path
		//get root
		//index through root
		//get next, etc
	//return directory (or error if not found);
	
	
	//update root
	read_block(0, (char *) superblock_buf);
	inode_buf = read_inode(superblock_buf->iroot);
	read_block(inode_buf->di_blocks[0], (char *) root);
	
	struct dir* curdir = root;
	char *token = strtok(path, delim);
	token = strtok(NULL, delim); 	//eliminate leading '/'
	
	while(token != NULL ){		//iterate path
		char comp_filename[NAMESIZE];
		int i = 1;	//i starts at 0 since every dirs entry 0 is itself.
		
		for(; i < 16;i++){		//linear search through dir to get filename
			strncpy(comp_filename, curdir->filename[i], NAMESIZE);
			
			//name matches, if directory, go into and break or just return immediately
			if(0 == strncmp(token, comp_filename, NAMESIZE)){
				*inode_index = curdir->inode_num[i];
				inode_buf = read_inode(*inode_index);
				if(inode_buf->flags == 0) return inode_buf;	//is a file, return its inode
				read_block(inode_buf->di_blocks[0], (char *)curdir)	//is a dir, go inside
				break;
			}
		}
		//reached end of dir without finding the dir we are looking for (we can iterate even if the dir does not have 16 entries, null values are ok to check against.)
		if(i==16){
			printf("Can not find directory at %s\n", path);
			return 0;
		}
		//at this point, we are one level deeper, so advance the token
		token = strtok(NULL, delim);
	}
	//if this loop terminates, curdir is the directory we are looking for and inode_index will still be correct.
	//safe to return since read_inode allocated the memory and we are still passing up a pointer to it.
	return inode_buf;
}