#include "libDisk.h"
#include "tfs.h"
#include <sys/stat.h>
#include <string.h>
#include "tinyFS_errno.h"

#define BLOCKTYPE_IDX 0
#define MAGIC_IDX 1
#define NEXT_ADDR_IDX 2
#define EMPTY_IDX 3

/*
  Note: table[open_file_idx] = offset_in_tinyFS
  < 0 : File Descriptor not opened.
*/
fileDescriptor file_descriptor_table[FILE_DESCRIPTOR_LIMIT] = {-1};


int tfs_mkfs(char *filename, int nBytes) {
  /*
    Makes a blank TinyFS file system of size nBytes and mount it onto
    the unix "filename" file. 
  */

  // initializing superblock of new file system
  superblock sb;
  sb.meta_data[BLOCKTYPE_IDX] = 1;
  sb.meta_data[MAGIC_IDX] = 0x44;
  sb.meta_data[NEXT_ADDR_IDX] = 0;
  sb.meta_data[EMPTY_IDX] = 0;

  // initializing free blocks;
  free_blocks *fb;
  

  disk_fd = openDisk(filename, nBytes);
  

  return 0;
}

int tfs_mount(char *diskname) {
  return 0;
}

fileDescriptor tfs_openFile(char *name) {

  return 0;
}

int tfs_closeFile(fileDescriptor FD) {
  return 0;
}

int tfs_writeFile(fileDescriptor FD,char *buffer, int size) {
  return 0;
}

int tfs_deleteFile(fileDescriptor FD) {
  return 0;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
  return 0;
}

int tfs_seek(fileDescriptor FD, int offset) {
  return 0;
}