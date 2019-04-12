#inlcude "diskcontroller.h"

int init(){
	FILE *f = fopen("./vdisk", "wb");
	fseek(f, MB2 , SEEK_SET);
	fputc(0, f);
	fclose(f);
	return 0;
}

char* read_block (short blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r");
	fseek(f, blocknum * BLOCKSIZE, SEEK_SET);
	fread(block_buf, BLOCKSIZE, 1, f);
	fclose(f);
	return block_buf;
}

int write_block(short blocknum, char* block_buf){
	FILE *f = fopen(vdisk, "r+");
	fseek(f, blocknum * BLOCKSIZE ,SEEK_SET);
	fwrite(block_buf, BLOCKSIZE, 1, f);
	fclose(f);
	return 0;
}