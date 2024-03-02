#define BLOCKSIZE 256 // size of disk block in bytes

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define OFLAGS O_WRONLY | O_CREAT | O_TRUNC
#define RWPERMS S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

int total_disk_space;
char default_file = "default";

int openDisk(char *filename, int nBytes);
int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
void writeBlock(int disk, int bNum, void *block);