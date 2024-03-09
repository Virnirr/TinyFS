#include "libDisk.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "TinyFS_errno.h"

int openDisk(char *filename, int nBytes) {
  /* Opens a regular UNIX file and designates the first nBytes, which is multiple of 
     of BLOCKSIZE into "filename". 
    
    @returns int disk_fd: the file descriptor designated by the OS to filename when opened.
    */
    int disk_fd;
    char *buffer;
    int total_byte;

    if (nBytes < BLOCKSIZE && nBytes > 0) {
      perror("nBytes is less than BLOCKSIZE failure");
      return OPENDISKERROR;
    } 

    // If nBytes is not multiple of BLOCKSIZE, set it as multiple of BLOCKSIZE lower than nBytes, but > 0
    if (nBytes % BLOCKSIZE) {
      nBytes = nBytes - (nBytes % BLOCKSIZE);
    }

    if (nBytes > BLOCKSIZE) {
      // open or create filename, truncate if needed
      disk_fd = open(filename, OFLAGS_OVER_WRITE, RWPERMS);
      if (disk_fd < 0) {
        perror("open disk filename");
        return OPENDISKERROR;
      }
      total_byte = sizeof(char) * nBytes;
      // Write to filename nBytes of 0 to specify size of disk in memory
      // Prepare a block of zeros
      buffer = (char *)malloc(total_byte);
      memset(buffer, 0, total_byte); // This initializes all elements to 0

      // Write the block of zeros to the file
      size_t written = write(disk_fd, buffer, total_byte);
      if (written < total_byte) {
        perror("Failed to write zeros to file");
        return OPENDISKERROR;
      }
      free(buffer); // free buffer since it's only one time use
    }

    // If nBytes is 0, an existing disk is opened, and don't overwrite content (i.e. append)
    if (nBytes == 0) {
      // need to verify that the unix file is correct
      disk_fd = open(filename, OFLAGS_EXIST, RWPERMS);
      if (disk_fd < 0) {
        perror("open disk with 0 bytes");
        return OPENDISKERROR;
      }
    }

    return disk_fd;
}



int closeDisk(int disk) {
  /*
  Takes a file descriptor disk and close it
  
  @param int disk: file descriptor of an open file
  @return int: returns 0 on Success, -1 on failure
  */

  // close tar file 
  if (close(disk) < 0) {
    perror("close");
    return CLOSEDISK_FAIL;
  }
  return 0;
}

int readBlock(int disk, int bNum, void *block) {
  /*  reads an entire block of BLOCKSIZE bytes from the open
  disk (identified by ‘disk’) and copies the result into a local buffer
  (must be at least of BLOCKSIZE bytes)
  
  @params int disk: file descriptor relating to a disk
  @params int bNum: a logical block number (bNum = 0 is very first byte of the file)
  @params void *block: 

   On success, it returns 0. 
   -1 or smaller is returned if disk is not available (hasn’t been opened) or any other failures. 
   You must define your own error code system. 
  */

  struct stat buf;
  if (fstat(disk, &buf) < 0) {
    perror("fstat");
    return -1;
  }

  int max_file_size = buf.st_size; // size  of regular file


  // seeks to the logical number position on disk
  int byte_offset = bNum * BLOCKSIZE;

  if (max_file_size < byte_offset) {
    perror("Max File Size Limit");
    return -1;
  }

  // read block position
  if (lseek(disk, byte_offset, SEEK_SET) < 0) {
    perror("lseek readBlock");
    return -1;
  }

  // reads from disk of the logical position and copies the result into buffer block
  int num_read = read(disk, block, BLOCKSIZE);
  if (num_read < 0) {
    perror("read");
    return -1;
  }

  return 0;
}

int writeBlock(int disk, int bNum, void *block) {
  /*
  Takes diks number (fd) and logical number bNum and writes the content of the buffer "block" to that location

  Returns 0 on success, or -1 on failure
  */

  struct stat buf;
  if (fstat(disk, &buf) < 0) {
    perror("fstat");
    return -1;
  }

  int max_file_size = buf.st_size; // size  of regular file

  // seeks to the logical number position on disk
  int byte_offset = bNum * BLOCKSIZE;

  if (max_file_size < byte_offset) {
    perror("Max File Size Limit");
    return -1;
  }

  // write block position
  if (lseek(disk, byte_offset, SEEK_SET) < 0) {
    perror("lseek writeBlock");
    return -1;
  }

  // write the block to position bNum in disk
  int num_write = write(disk, block, BLOCKSIZE);
  if (num_write < BLOCKSIZE) {
    perror("write");
    return -1;
  }

  return 0;
}