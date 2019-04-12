#include "file.h"

int main(){
	init_fs();
	
	mkdir("/test1");
	mkdir("/test2");
	mkdir("/test1/test1.1");
	mkdir("/test2/test2.2");
	
	rmdir("/test2/test2.2");
	rmdir("/test2");
	
	FILE *f = fopen("test.txt", r);
	
	write_file("test1/test1.1/test_file.txt", f);
	write_file("test1/test_file2.txt", f);
	
	read_file("test1/test1.1/test_file.txt");
	read_file("test1/test_file2.txt");
	
	del_file("test1/test1.1/test_file.txt")
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
	free_vector[1] = 0b11000000;
	write_block(1, (char*) &free_vector);

	//make root
	char root_i_index = 0;
	short root_block = 10;
	
	struct inode* root_inode = (struct inode*) malloc(sizeof(struct inode));
	root_inode->flags = 1;
	root_inode->di_blocks[0] = root_block;
	root_inode->size = BLOCKSIZE;
	
	struct dir* root_dir = (struct dir*) malloc(sizeof(struct dir));
	root_dir->inode_num[0] = 10;
	strncat(".", root_dir->filename[0]);
	
	write_block(root_block, (char *) root_dir);
	claim_block(root_block);
	
	write_inode(root_inode, root_i_index);
	claim_inode(root_i_index);


	return 0;
}


//find where to write file
//prep inodes
//parse through datablock chunks
//write changes
//todo: size
int write_file(char *path, FILE *f){
	//separates out the existing path from the new filename (could make this a function but I dont want to)
	int len = strlen(path);
	char *filename = strrchr(path, '/');
	filename++;
	int filenamelen = strlen(filename);
	char pathname[(len-filenamelen) + 1];
	strncpy(pathname, path, len-filenamelen);
	
	//get parent dir inode/datablock
	char parentInode_index;
	struct inode* parentInode = getfile_inode(pathname, &parentInode_index);	//freed later
	struct dir* parentDir;
	read_block(parentInode->di_blocks[0], (char *)parentDir);
	
	//find empty entry in parent dir or overwrites existing file of same name
	int i;
	for (i = 1; i < 16 || inode_num[i] == 0 || strncmp(filename, parentDir->filename[i], NAMESIZE) != 0; i++);
	if(i == 16){
		printf("Directory %s is full, cannot add new file.\n", pathname);
		return -1;
	}
	
	//delete old copy if it exists
	if(inode_num[i] != 0){
		del_file(char *path);
	}
	
	//get new inode for the new file
	char nfileInodeIndex = get_inode();
	struct inode* nfileInode = read_inode(nfileInodeIndex); //freed later
	
	//update parent
	parentDir->inode_num[i] = nfileInodeIndex;
	strncpy(filename, parentDir->filename[i], NAMESIZE);
	
	//fill out new file inode
	int size = fread(block_buf, BLOCKSIZE, 1, f);
	short t_block = get_block();
	//direct blocks
	for(int i = 0; i < 10; i++, t_block = get_block(), size = fread(block_buf, BLOCKSIZE, 1, f)){
		if(t_block == -1) return -1;
		if(size == 0) return 0;
		write_block(t_block, block_buf);
		nfileInode->di_blocks[i] = t_block;
		claim_block(t_block);
	}
	
	
	//single indirect
	struct indirection_block* ib_buf = (struct indirection_block*) malloc(sizeof(indirection_block));
	t_block = get_block();
	if(t_block == -1) return -1;
	nfileInode->I_block = t_block;

	
	//update indirect block
	t_block = get_block();
	size = fread(block_buf, BLOCKSIZE, 1, f);
	for(int i = 0; i < 256; i++, t_block = get_block(), size = fread(block_buf, BLOCKSIZE, 1, f)){
		if(t_block == -1) return -1;
		if(size == 0) break;
		write_block(t_block, block_buf);
		ib_buf->block[i] = t_block;
		claim_block(t_block);
	}
	//write it to disk
	write_block(nfileInode->I_block, (char *) ib_buf);
	claim_block(nfileInode->I_block);
	free(ib_buf);
	//if (size == 0) return 0;//dont bother with double
	
	
	//double indirect
	struct indirection_block* dib_buf = (struct indirection_block*) malloc(sizeof(indirection_block));
	tblock = get_block();
	if(t_block == -1) return -1;
	nfileInode->DI_block = t_block;
	
	t_block = get_block();
	for(int i = 0; i < 256 && size != 0; i++){
		if(t_block == -1) return -1;
		ib_buf = (struct indirection_block*) malloc(sizeof(indirection_block));
		dib_buf->block[i] = t_block;
		
		t_block = get_block();
		size = fread(block_buf, BLOCKSIZE, 1, f);
		//single indirect again
		for(int j = 0; j < 256 && size != 0; j++, t_block = get_block(), size = fread(block_buf, BLOCKSIZE, 1, f)){
			if(t_block == -1) return -1;
			if(size == 0) break;
			write_block(t_block, block_buf);
			ib_buf->block[i] = t_block;
			claim_block(t_block);
		}
		
		write_block(dib_buf->block[i], (char*) ib_buf);
		claim_block(dib_buf->block[i]);
		free(ib_buf);
	}
	write_block(nfileInode->DI_block, dib_buf);
	claim_block(nfileInode->DI_block);
	free(dib_buf);
	
	
	
	//write out new file inode
	write_inode(nfileInode, nfileInodeIndex);
	claim_inode(nfileInodeIndex);
	
	//update parent dir data.
	write_block(parentInode->di_blocks[0], (char*) parentDir);
	
	//free memory
	free(nfileInode);
	free(parentInode);
	
	return 0;
}

//takes the file at given path and copies it into the real filesystem in the directory this file is stored in.
int read_file(char *path){
	int len = strlen(path);
	char *filename = strrchr(path, '/');
	filename++;
	int filenamelen = strlen(filename);
	char pathname[(len-filenamelen) + 1];
	strncpy(pathname, path, len-filenamelen);
	
	//get parent
	char parentInode_index;
	struct inode* parentInode = getfile_inode(pathname, &parentInode_index);		//free later
	struct dir* parentDir;
	read_block(parentInode->di_blocks[0], (char *)parentDir);
	
	//find entry in parent dir
	int i;
	for (i = 1; i < 16 || strncmp(filename, parentDir->filename[i], NAMESIZE) != 0; i++);
	if(i==16){
		printf("Cannot find file %s.\n", pathname);
		return -1;
	}
	
	struct inode* f_inode = read_inode(parentDir->inode_num[i]);		//free later
	
	//copy the data blocks out to the file.
	FILE *f = fopen(filename, "w");
	
	
	//read direct, terminate on 0
	int i;
	for(i = 0; i < 10; i++){
		if(f_inode->di_blocks[i] == 0) break;
		read_block(f_inode->di_blocks[i], block_buf);
		fwrite(block_buf, BLOCKSIZE, 1, f);
	}
	if(f_inode->di_blocks[i] == 0){
		fclose(f);
		return 0;
	}
	//read single indirect, terminate on 0
	struct indirection_block* ib_buf;
	read_block(f_inode->I_block, (char *) ib_buf);
	for(i = 0; i < 256; i++){
		if(ib_buf->block[i] == 0) break;
		read_block(ib_buf->blocks[i], block_buf);
		fwrite(block_buf, BLOCKSIZE, 1, f);
	}
	if(ib_buf->block[i] == 0){
		fclose(f);
		return 0;
	}
	
	//read double indirect, terminate on 0
	struct indirection_block* dib_buf;
	read_block(f_inode->DI_block, (char*) dib_buf);
	for(i = 0; i < 256; i++){
		if(dib_buf->block[i] == 0){
			fclose(f);
			return 0;
		}
		read_block(dib_buf->block[i], ib_buf);
		int j;
		for(j = 0; j < 256; j++){
			if(ib_buf->block[j] == 0) return 0;
			read_block(ib_buf->block[j], block_buf);
			fwrite(block_buf, BLOCKSIZE, 1, f);
		}
		if(ib_buf->block[j] == 0){
			fclose(f);
			return 0;
		}
	}
	
	//no editing happened, only reads so no writes necessary
	free(parentInode);
	free(f_inode);
	
	return 0;
}

int del_file(char *path){
	return rmdir(path);
}

// resolve path (first assume every dir created in root)
// create directory
	// get inode
	// get data block(s)
	// write data block(s)
	// write inode
// update parent directory(root)
int mkdir(char *path){
	//separates out the existing path from the new filename (could make this a function but I dont want to)
	int len = strlen(path);
	char *filename = strrchr(path, '/');
	filename++;
	int filenamelen = strlen(filename);
	char pathname[(len-filenamelen) + 1];
	strncpy(pathname, path, len-filenamelen);
	
	
	//for example, if path was '/dir1/subdir1/test.txt'
	//filename now is 'test.txt'
	//pathname now is '/dir1/subdir1'	

	
	//get new directory inode
	char dirInodeIndex = get_inode();
	short dirBlock = get_block(); 
	
	//get new directory data block
	struct inode* curdir = (struct dir*) malloc(sizeof(struct dir));
	struct inode dirInode = read_inode(dirInodeIndex);
	
	//set values for new directory datablock and inode
	dirInode->size = BLOCKSIZE;
	dirInode->di_blocks[0] = dirBlock;
	dirInode->flags = 1
	
	curdir->inode_num[0] = dirInodeIndex;
	strncpy(".", curdir->filename[0], NAMESIZE);
	
	//get parent dir inode/datablock
	char parentInode_index;
	struct inode* parentInode = getfile_inode(pathname, &parentInode_index);
	struct dir* parentDir;
	read_block(parentInode->di_blocks[0], (char *)parentDir);
	
	//find empty entry in parent dir
	int i;
	for (i = 1; i < 16 || parentDir->inode_num[i] != 0; i++);
	if(i==16){
		printf("Directory %s is full, cannot add new file.\n", pathname);
		return -1;
	}
	
	//fill empty entry in parent dir with proper data
	parentDir->inode_num[i] = dirInodeIndex;
	strncpy(filename, parentDir->filename[i], NAMESIZE)
	
	
	//write new dir changes to disk
	write_block(dirBlock, (char *) curdir);
	claim_block(dirBlock);
	write_inode(dirInode, dirInodeIndex);
	claim_inode(dirInode);
	
	//write updated parent to disk
	write_block(parentInode->di_blocks[0], (char*) parentDir);
	
	//free heap memory (any explicit malloc, or calls to read_inode/getfile_inode return dynamic)
	free(curdir);
	free(dirInode);
	free(parentInode)
	return 0;
}

int rmdir(char *path){
	//resolve pathname
	//update free vectors
	//remove entry from parent dir
	if(strlen(path) == 1){
		printf("Cannot delete root.\n");
		return -1;
	}
	
	//separates out the existing path from the new filename (could make this a function but I dont want to)
	int len = strlen(path);
	char *filename = strrchr(path, '/');
	filename++;
	int filenamelen = strlen(filename);
	char pathname[(len-filenamelen) + 1];
	strncpy(pathname, path, len-filenamelen);
	
	//get parent dir info
	char parentInode_index;
	struct inode* parentInode = getfile_inode(pathname, &parentInode_index);
	struct dir* parentDir;
	read_block(parentInode->di_blocks[0], (char *)parentDir);
	
	//find entry in parent dir
	int i;
	for (i = 1; i < 16 || strncmp(filename, parentDir->filename[i], NAMESIZE) != 0; i++);
	if(i==16){
		printf("Cannot find file %s.\n", pathname);
		return -1;
	}
	//save value to free later;
	char inode_index_to_free = parentDir->inode_num[i];
	
	//get block indecies to free later
	struct inode* inode_to_free= read_inode(inode_index_to_free);
	
	//update parent
	parentDir->inode_num[i] = 0;
	parentDir->filename[i] = "";
	
	write_block(parentInode->di_blocks[0], parentDir);
	
	//update free vectors
	free_inode(inode_index_to_free);
	
	//free direct blocks, terminate whole deletion on 0 vector (file done)
	for(int i = 0; i < 10; i++){
		if(inode_to_free->di_blocks[i] == 0) return 0;
		free_block(inode_to_free->di_blocks[i]);
	}
	//free single indirect, terminate on 0
	struct indirection_block* ib_buf;
	read_block(inode_to_free->I_block, (char *) ib_buf);
	for(int i = 0; i < 256; i++){
		if(ib_buf->block[i] == 0) return 0;
		free_block(ib_buf->block[i]);
	}
	
	//free double indirect, terminate on 0
	struct indirection_block* dib_buf;
	read_block(inode_to_free->DI_block, (char*) dib_buf);
	for(int i = 0; i < 256; i++){
		if(dib_buf->block[i] == 0) return 0;
		read_block(dib_buf->block[i], ib_buf);
		for(int j = 0; j < 256; j++){
			if(ib_buf->block[j] == 0) return 0;
			free_block(ib_buf->block[j]);
		}
	}
	return 0;
}

// tree(){
	
// }