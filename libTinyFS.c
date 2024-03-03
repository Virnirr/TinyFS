#include "libDisk.h"
#include "tfs.h"
#include <sys/stat.h>
#include <string.h>
#include "tinyFS_errno.h"
#include <limits.h>

#define BLOCKTYPE_IDX 0
#define MAGIC_IDX 1
#define NEXT_ADDR_IDX 2
#define EMPTY_IDX 3


/*
  Note: table[open_file_idx] = offset_in_tinyFS
  < 0 : File Descriptor not opened.
*/

file_pointer file_descriptor_table[FILE_DESCRIPTOR_LIMIT] = {NULL}:

int curr_fs_fd;

int tfs_mkfs(char *filename, int nBytes) {
  /*
    Makes a blank TinyFS file system of size nBytes and mount it onto
    the unix "filename" file. 
  */
  int disk_error;
  // Maximum number of blocks: 2,147,483,647
  // Max storage: 2,147,483,647 blocks * 256 bytes =  550 GBs
  if ((int) (nBytes / BLOCKSIZE) > INT_MAX) {
    perror("More than INT_MAX");
    return EMAX_INT; // returns -7
  }

  if ((int) (nBytes / BLOCKSIZE) < 2) {
    perror("Too Small");
    return EMINLIMIT;
  }

  // initializing superblock of new file system
  superblock sb;
  sb.block_type = SB_CODE;
  sb.magic_num = MAGIC_NUM;
  sb.address_of_root = 1; // setting as 1 since logical offset 1
  sb.next_free_block = 2;
  sb.rest = {0}; // initialize the rest as 0

  // initializing free blocks;
  free_blocks *fb;

  disk_fd = curr_fs_fd = openDisk(filename, nBytes);
  
  // write superblock into first block of file system
  if ((disk_error = writeBlock(disk_fd, 0, &sb)) < 0) {
    perror("write block");
    return disk_error;
  }
  
  int fs_idx;
  // initialize free block
  free_blocks fb;
  fb.block_type = FB_CODE;
  fb.magic_num = MAGIC_NUM;
  fb.rest = {0}; // initialize the rest as 0

  for (fs_idx = 2; fs_idx < (int) (nBytes / BLOCKSIZE) - 1; fs_idx++) {
    // initialize next free block as linked list and write it into file system
    fb.next_fb = fs_idx + 1;
    if ((disk_error = writeBlock(disk_fd, fs_idx, &fb)) < 0) {
      perror("write block");
      return disk_error;
    }
  }
  // last free block points to -1
  fb.next_fb = -1;
  if ((disk_error = writeBlock(disk_fd, fs_idx, &fb)) < 0) {
    perror("write block");
    return disk_error;
  }
  return 0;
}
//“mounts” a TinyFS file system located within ‘diskname’
int tfs_mount(char *diskname) {
  int disk_fd;

  // open existing disk
  if ((disk_fd = openDisk(diskname, 0)) < 0) {
    perror("mount");
    return disk_fd; // this will be the error number
  }

  int num_read;
  char buffer[BLOCKSIZE];
  uint8_t TFS_buffer[BLOCKSIZE];
  // read superblock of current system
  if (readBlock(curr_fs_fd, 0, &TFS_buffer) < 0) {
    perror("readBlock")
    return READBLOCK_FAIL;
  }
  free_index = convert_str_to_int(TFS_buffer, 6, 9);
  // make sure the new disk is a valid file system
  while ((num_read = read(curr_fs_fd, buffer, BLOCKSIZE)) > 0) {
    // make sure every magic number is correct
    if (buffer[MAGIC_IDX] != MAGIC_NUM) {
      return NOT_A_FILE_SYSTEM;
    }
    // writing to new fd
    if (writeBlock(disk_fd, free_index, buffer) < 0){
      perror("write");
      return -1;
    }
    //fix dis 
    if (readBlock(curr_fs_fd, free_index, &TFS_buffer) < 0) {
      perror("readBlock")
      return READBLOCK_FAIL;
    }
    free_index = convert_str_to_int(TFS_buffer, 2, 5);
     
  }
  curr_fs_fd = disk_fd;
  return 0;
}

int tfs_unmount(void) {
  /* unmounts the current file system */

  if (curr_fs_fd < 0)
  {
    perror("unmount");
    return UMOUNT_FAIL;
  }
  closeDisk(curr_fs_fd);
  curr_fs_fd = -1;
  return 0;
}

fileDescriptor tfs_openFile(char *name) {

  unsigned long fd_idx = hash(name) % FILE_DESCRIPTOR_LIMIT;
  // file is open and exist in file descriptor table
  if (file_descriptor_table[fd_idx] != -1) {
    return file_descriptor_table[fd_idx];
  }
  int num_read;
  /* rest read pointer */
  if (lseek(in_fd, 0, SEEK_SET) == -1) {
      perror("lseek");
      exit(EXIT_FAILURE);
  }

  char buffer[BLOCKSIZE];
  int file_fd;
  int logical_disk_offset = 0;
  // find it with linear probing (assuming it's not a directory right now)
  while (num_read = read(disk, buffer, BLOCKSIZE) > 0) {
    // if it's an inode and the name matches, then you cache it in open directory file
    if (buffer[BLOCKTYPE_IDX] == INODE_CODE && !strcmp(filename, name)) {
      
      // find a location that exist and set that location as the logical disk offset.
      file_fd_idx = hash(name);
      while (file_descriptor_table[file_fd_idx] != -1) {
        file_fd_idx++;
      }
      file_descriptor_table[file_fd_idx] = logical_disk_offset;
      break;
    }
    logical_disk_offset++;
  }
  if (num_read < 0) {
    perror("read");
    return -1;
  }

  return 0;
}

int tfs_closeFile(fileDescriptor FD) {
  /* Closes the file, de-allocates all system resources, 
     and removes table entry) */
  
  if (file_descriptor_table[FD] == -1) {
    return EBADF;
  }
  
  file_descriptor_table[FD] = -1;

  return 0;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
  /* Writes buffer ‘buffer’ of size ‘size’, which represents an entire
file’s content, to the file system.  */
  file_extent fe;
  fe.block_type = FE_CODE;
  fe.magic_num = MAGIC_NUM;
  fe.next_fe = -1; // signalifying no more bytes
  char size_temp[FILE_SIZE_TEMP] = {'\0'};
  char first_file_extent[FILE_SIZE_TEMP] = {'\0'};
  int file_size, file_content_offset;

  uint8_t TFS_buffer[BLOCKSIZE];

  // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  /* the next free block in offset  */
  next_free_block_offset = convert_str_to_int(TFS_buffer, 6, 10);

  int file_offset = file_descriptor_table[FD];
  
  // get the inode of file to check next file extent
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  file_content_offset = convert_str_to_int(TFS_buffer, 40, 43);

  // There is no next file extent in the file inode. Allocate new memory for it.
  if (file_content_offset == -1) {
    disk_offset = next_free_block;
    // copy up to FILE_EXTENT_DATA_LIMIT into fe.data
    strncpy(fe.data, buffer, FILE_EXTENT_DATA_LIMIT);
    size -= FILE_EXTENT_DATA_LIMIT;
  }
  
  // write content into disk
  if (writeBlock(FD, disk_offset, fe) < 0) {
    perror("Disk Write");
    return -1
  }

  while (size > 0) {
    // write more if there isn't enough
  }

  return 0;
}

int tfs_deleteFile(fileDescriptor FD) {
  /* Takes a file descriptor and remove the file (set it as free block) in the tinyFS */

  int file_inode_offset = file_descriptor_table[FD];
  
  uint8_t TFS_buffer[BLOCKSIZE];

  // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, file_inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }

  int file_content_offset = convert_str_to_int(TFS_buffer, 40, 43);

  // there's content, so set it to free block
  while (file_content_offset != -1) {
    // get the superblock check for first free block
    if ((disk_error = readBlock(curr_fs_fd, file_content_offset, TFS_buffer)) < 0) {
      return disk_error;
    }
    // set it to next content offset (if there is any) so you can free that one too
    file_content_offset = convert_str_to_int(TFS_buffer, 2, 5);
    if (set_block_to_free(file_content_offset) < 0) {
      perror("set block to free");
      return -1;
    }
  }

  return 0;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
  /* reads one byte from the file and copies it to buffer, using the
  current file pointer location and incrementing it by one upon success. */
  int curr_file_pointer = file_descriptor_table[FD];
  int pointer = curr_file_pointer.pointer;
  int next_fe_offset = curr_file_pointer.next_file_extent_offset;
  int curr_fe_offset = curr_file_pointer.curr_file_extent_offset;

  // File pointer has already past the end of the file (i.e. > FILE_EXTENT_DATA_LIMIT and no next file extent offset)
  if (curr_file_pointer - FILE_EXTENT_META_DATA_SIZE > FILE_EXTENT_DATA_LIMIT 
      && next_fe_offset == -1) {
    perror("end of file");
    return TFS_EFO;
  }

  // buffer that stores the file extent contents
  uint8_t TFS_buffer[BLOCKSIZE];

  // read in the current file extent block
  if ((disk_error = readBlock(curr_fs_fd, curr_fe_offset, TFS_buffer)) < 0) {
    return disk_error;
  }

  // copy the one byte content into buffer
  strncpy(buffer, &TFS_buffer[pointer], 1);

  // increment current file pointer to the next byte;
  curr_file_pointer.pointer++;

  // end of first file pointer and 
  if (curr_file_pointer - FILE_EXTENT_META_DATA_SIZE > FILE_EXTENT_DATA_LIMIT) {
    
  }

  return 0;
}

int tfs_seek(fileDescriptor FD, int offset) {
  /* implement seek based off of tfs_readByte */
  return 0;
}

// djb2 hash function for strings
unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int convert_str_to_int(char *buffer, int start, int end) {
  /* Takes as input */
  int i, j = 0;
  for (i = start; i < end + 1; i++) {
      size_temp[j++] = buffer[i];
  }
  return strtol(size_temp, NULL, BASE_TEN);
}

int set_block_to_free(int offset) {
  /* Uses a stack structure LIFO. Util function to set block to free. */

  free_block fe;
  fe.block_type = FE_CODE;
  fe.magic_num = MAGIC_NUM;

    // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  /* the next free block in offset  */
  next_free_block_offset = convert_str_to_int(TFS_buffer, 6, 10);

  fe.next_fb = next_free_block_offset; // set it to first
  
    // write content into disk
  if (writeBlock(curr_fs_fd, offset, fe) < 0) {
    perror("Disk Write");
    return -1
  }

  return 0;
}