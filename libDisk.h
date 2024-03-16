#define BLOCKSIZE 256 // size of disk block in bytes

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define OFLAGS_EXIST O_RDWR
#define OFLAGS_OVER_WRITE  O_RDWR | O_CREAT | O_TRUNC
#define RWPERMS      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define FILE_NAME_SIZE 9


int openDisk(char *filename, int nBytes);
int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);

// typedef openDiskList* open_disk
// typedef struct open_disk_list {
//   int disk_fd;
//   char filename[FILE_NAME_SIZE];
//   open_disk next_disk;
// } open_disk_list;

int diskTest();