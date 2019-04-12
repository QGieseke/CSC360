#include "filecontroller.c"


int mkdir(char *path);
int rmdir(char *path);


int write_file(char *path, FILE *f);
int read_file(char *path);
int del_file(char *path);

int init_fs();

tree();