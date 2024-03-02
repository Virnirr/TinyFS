#define BLOCKSIZE 256 // size of disk block in bytes

int openDisk(char *filename, int nBytes);
int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
void writeBlock(int disk, int bNum, void *block);