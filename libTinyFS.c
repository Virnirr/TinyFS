#include "libDisk.h"
#include "tinyFS.h"
#include <sys/stat.h>
#include <string.h>
#include "TinyFS_errno.h"
#include <limits.h>

#define BLOCKTYPE_IDX 0
#define MAGIC_IDX 1
#define NEXT_ADDR_IDX 2
#define EMPTY_IDX 3


#define ROOT_ADDRESS_IDX 1
#define NEXT_FREE_BLOCK_IDX 2

/*
  Note: table[open_file_idx] = offset_in_tinyFS
  < 0 : File Descriptor not opened.
*/
file_pointer file_descriptor_table[FILE_DESCRIPTOR_LIMIT] = {NULL}:

int curr_fs_fd;


/* ------------------------------------ Main TinyFS library functions ------------------------------------ */

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
  sb.address_of_root = ROOT_ADDRESS_IDX; // setting as 1 since logical offset 1
  sb.next_free_block = NEXT_FREE_BLOCK_IDX; // starts at 2
  sb.rest = {0}; // initialize the rest as 0

  // initializing free blocks;
  free_blocks *fb;

  // open new filename
  disk_fd = curr_fs_fd = openDisk(filename, nBytes);
  
  // write superblock into first block of file system
  if ((disk_error = writeBlock(disk_fd, 0, &sb)) < 0) {
    perror("write block");
    return disk_error;
  }
  // write the root inode into the second block in TinyFS
  inode root;
  fill_new_inode_buffer(&root *inode_buffer, DIR_CODE, NULL); // only root will have filename = NULL
  // root.block_type = INODE_CODE;
  // root.magic_num = MAGIC_NUM;
  // root.file_type = DIR_CODE;
  // root.file_size = 0; // directory has file size of 0

  // Get current time as time_t object
  // time_t now = time(NULL);
  // root.creation_time = now;
  // root.access_time = now;
  // root.modification_time = now;
  root.first_file_extent = ROOT_POS; // position of where to find the child directories in hierachical
  // set filename to NULL for root nodes
  // memset(root.filename, '\0', FILENAME_SIZE);
  // copy "/" into prefix and append with rest with NULL or \0
  // strncpy(root.prefix, "/", PREFIX_SIZE);
  
  int fs_idx = ROOT_POS; // 1
  
  // write the root block into TinyFS
  if ((disk_error = writeBlock(disk_fd, fs_idx, &root)) < 0) {
    perror("write block");
    return disk_error;
  }
  
  // // This file extent for directory will store all the root child's index position in the TinyFS
  // file_extent fe;
  // fe.block_type = FE_CODE;
  // fe.magic_num = MAGIC_NUM;
  // fe.next_fe = -1;
  // // set the data to all NULL
  // memset(fe.data, '\0', FILE_EXTENT_DATA_LIMIT);

  // write root block's file extent into TinyFS
  if ((disk_error = writeBlock(disk_fd, fs_idx, &root)) < 0) {
    perror("write block");
    return disk_error;
  }
  
  // initialize free block
  free_blocks fb;
  fb.block_type = FB_CODE;
  fb.magic_num = MAGIC_NUM;
  fb.rest = {0}; // initialize the rest as 0

  // fill the rest of the memory as free blocks
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

  // return 0 on success
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

  // make sure that every block is valid TinyFS using magic number check
  int tinyfs_offset_idx;
  struct stat buf;
  char TFS_buffer[BLOCKSIZE];

  if (fstat(disk, &buf) < 0) {
    perror("fstat");
    return -1;
  }

  int max_file_size = buf.st_size; // size  of regular file

  // if file size is less than BLOCKSIZE or not a multiple of BLOCKSIZE, then it's not a TinyFS 
  if (max_file_size < BLOCKSIZE || max_file_size % BLOCKSIZE) {
    return NOT_A_FILE_SYSTEM;
  }
  // check every block to make sure it is compliant to TinyFS
  for (tinyfs_offset_idx = 0; tinyfs_offset_idx < (max_file_size / BLOCKSIZE); tinyfs_offset_idx++) {
    if (readBlock(disk_fd, tinyfs_offset_idx, TFS_buffer) < 0) {
      return READBLOCK_FAIL;
    }
    // if magic number does not match TinyFS magic number, then it's not valid
    if (TFS_buffer[MAGIC_IDX] != MAGIC_NUM) {
      return NOT_A_FILE_SYSTEM;
    }
  }
  // Unmount current TinyFS and mount the new one opened.
  int status;
  if ((status = unmount(curr_fs_fd) < 0))
  {
    //if unmount fails because of closeDisk
    if (status == CLOSEDISK_FAIL)
    {
      return CLOSEDISK_FAIL;
    }
  }
  curr_fs_fd = disk_fd; //set the curr fd to the one that was just mounted 

  return MOUNT_SUCCESS;
}

int tfs_unmount(void) {
  /* unmounts the current file system */

  if (curr_fs_fd < 0)
  {
    perror("unmount");
    return UMOUNT_FAIL;
  }
  if (closeDisk(curr_fs_fd) < 0)
  {
    return CLOSEDISK_FAIL;
  }
  curr_fs_fd = -1;
  return UNMOUNT_SUCCESS;
}

fileDescriptor tfs_openFile(char *name) {

  // unsigned long fd_idx = hash(name) % FILE_DESCRIPTOR_LIMIT;
  int fd_table_idx;
  file_pointer curr_fd_table_pointer;
  int logical_offset;
  int first_null_fd_idx = NULL;

  uint8_t TFS_buffer[BLOCKSIZE];
  char filename_buffer[FILENAME_SIZE];

  // check all the filedescriptor table to see if you can find the filename containing "name"
  for (fd_table_idx = 0; fd_table_idx < FILE_DESCRIPTOR_LIMIT; fd_table_idx++) {
    // if the fd_table_idx is not NULL, check to see if the inode filename matches parameter name.
    // if it does, return that file descriptor since it's already opened.

    // save the first null file descriptor table index for later when creating new file
    // so you don't have to loop twice.
    if (first_null_fd_idx == NULL && file_descriptor_table[fd_table_idx] == NULL) {
      first_null_fd_idx = fd_table_idx;
    }


    if (file_descriptor_table[fd_table_idx] != NULL) {

      curr_fd_table_pointer = file_descriptor_table[fd_table_idx];
      logical_offset = curr_fd_table_pointer.inode_offset;

      // read the block from the logical offset into TFS_buffer
      if ((disk_error = readBlock(curr_fs_fd, logical_offset, TFS_buffer)) < 0) {
        return disk_error;
      }

      // copy filename into filename_buffer from the inode to check if it's similar to name
      strncpy(filename_buffer, TFS_buffer + 3, FILENAME_SIZE);

      // it is correct, so return it
      if (!strcmp(name, filename_buffer)) {
        return fd_table_idx;
      }
    }
  }

  // if you are unable to find it, then find an empty space and allocate file pointer for "name" and write to it.

  // create new inode
  inode new_file_inode;
  fill_new_inode_buffer(&new_file_inode, FILE_CODE, name);

  // file is open and exist in file descriptor table
  // if (file_descriptor_table[fd_idx] != NULL) {
  //   return file_descriptor_table[fd_idx];
  // }

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
      // file_fd_idx = hash(name); do linear probe instead
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
  
  file_descriptor_table[FD] = NULL;

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


  int file_offset = file_descriptor_table[FD];
  
  // get the inode of file to check next file extent
  if ((disk_error = readBlock(curr_fs_fd, file_offset, TFS_buffer)) < 0) {
    return disk_error;
  }

  file_content_offset = convert_str_to_int(TFS_buffer, 40, 43);

  // free the current content so you restart over
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
    // will become -1 at the end when you have no next files
    file_content_offset = convert_str_to_int(TFS_buffer, 40, 43);
  }

  // write and save the first file extent to the inode
  int write_size = size;
  disk_offset = next_free_block_offset;
  
  // copy up to FILE_EXTENT_DATA_LIMIT into fe.data
  strncpy(fe.data, buffer, FILE_EXTENT_DATA_LIMIT);

  // subtract from total to keep track of how much write
  write_size -= FILE_EXTENT_DATA_LIMIT;
  // go to the next set of bytes to write
  buffer += FILE_EXTENT_DATA_LIMIT;
  
  // write content into disk
  if (writeBlock(FD, disk_offset, fe) < 0) {
    perror("Disk Write");
    return -1
  }
  
  // If there is still more, add it to the end of linked list of file extent
  while (write_size > 0) {

    // copy up to FILE_EXTENT_DATA_LIMIT into fe.data
    strncpy(fe.data, buffer, FILE_EXTENT_DATA_LIMIT);

    // subtract from total to keep track of how much write
    write_size -= FILE_EXTENT_DATA_LIMIT;
    // go to the next set of bytes to write
    buffer += FILE_EXTENT_DATA_LIMIT;
  }

  return 0;
}

int tfs_deleteFile(fileDescriptor FD) {
  /* Takes a file descriptor and remove the file (set it as free block) in the tinyFS */

  file_pointer fp_struct = file_descriptor_table[FD];
  
  uint8_t TFS_buffer[BLOCKSIZE];

  // get the superblock check for first file content block
  if ((disk_error = readBlock(curr_fs_fd, fp_struct.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  //gets content of inode
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
  // free the inode
  if (set_block_to_free(fp_struct.inode_offset) < 0) {
    perror("set block to free");
    return -1;
  }
  return 0;
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
  /* reads one byte from the file and copies it to buffer, using the
  current file pointer location and incrementing it by one upon success. */
  file_pointer curr_file_pointer = file_descriptor_table[FD];
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

  // end of data block for file pointer and there's still a data block, 
  // change curr_file_extent_offset and next_file_extent_offset
  if (curr_file_pointer - FILE_EXTENT_META_DATA_SIZE > FILE_EXTENT_DATA_LIMIT) {
    if (next_fe_offset != -1){
      int disk_error;
        if ((disk_error = readBlock(curr_fs_fd, next_fe_offset, TFS_buffer)) < 0) {
          return disk_error;
        }
      curr_fe_offset = next_fe_offset;
      next_fe_offset = convert_str_to_int(TFS_buffer, 2, 5);
    }
  }

  return 0;
}
// change the file pointer location to offset (absolute). 
// Returns success/error codes
int tfs_seek(fileDescriptor FD, int offset) {
  /* implement seek based off of tfs_readByte */

  file_pointer curr_file_pointer = file_descriptor_table[FD];
  // if fd doesn't exist
  if (curr_file_pointer == NULL)
  {
    perror("unopened file");
    return EBADF;
  }
  // if offset is invalid
  if (offset > curr_file_pointer.file_size || offset < 0){
    perror("offset");
    return OFFSET_FAIL;
  }

  int pointer = curr_file_pointer.pointer;
  pointer = offset % FILE_EXTENT_DATA_LIMIT
  //get curr_file_extent_offset
 
  uint8_t TFS_buffer[BLOCKSIZE];
  // initially sets curr_file_extent_offset to the first extent
  // and next_file_extent_offset to the one after first extent
  int read_inode;
  if ((read_inode = readBlock(FD, curr_file_pointer.inode_offset, TFS_buffer)) < 0) {
      return read_inode;
  }
  int curr_file_extent = convert_str_to_int(TFS_buffer, 40, 43);
  if ((read_inode = readBlock(FD, curr_file_extent, TFS_buffer)) < 0) {
      return read_inode;
  }
  int next_file_extent = convert_str_to_int(TFS_buffer, 2, 5);
  // then, if offset is larger than one file extent, loop through the file extents 
   int page_offset = (int) (offset / FILE_EXTENT_DATA_LIMIT);
  for (int i = 0; i < page_offset; i++)
  {
    curr_file_extent = next_file_extent;
    if ((read_inode = readBlock(FD, curr_file_extent, TFS_buffer)) < 0) {
    return read_inode;
    }
    next_file_extent = convert_str_to_int(TFS_buffer, 2, 5);
  }
  return 0;
}


/* ------------------------------------ Util Functions ------------------------------------ */


void fill_new_inode_buffer(inode *inode_buffer, int file_type, char *filename) {
  /* Fills in new inode blocks to write */
  inode_buffer->block_type = INODE_CODE;
  inode_buffer->magic_num = MAGIC_NUM;
  inode_buffer->file_type = file_type;
  strncpy(inode_buffer->filename, filename, FILENAME_SIZE);
  inode_buffer->file_size = 0;

  // Get current time as time_t object
  time_t now = time(NULL);
  inode_buffer->creation_time = now;
  inode_buffer->access_time = now;
  inode_buffer->modification_time = now;

  inode_buffer->first_file_extent = -1;
  inode_buffer->parent_inode = -1;
  
  // set all children to -1 (i.e. no children)
  memset(inode_buffer->child, -1, MAX_CHILD);
}


int convert_str_to_int(char *buffer, int start, int end) {
  /* Takes as input */
  int i, j = 0;
  char int_in_str[SIZE_OF_INT_IN_STR];

  strncpy(int_in_str, buffer+start, SIZE_OF_INT_IN_STR - 1);
  int_int_str[SIZE_OF_INT_IN_STR] = '\0';
  // for (i = start; i < end + 1; i++) {
  //     size_temp[j++] = buffer[i];
  // }
  return strtol(int_in_str, NULL, BASE_TEN);
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

int get_next_free_and_set() {
  /* get next free block and set it to the free block after it*/

  // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  /* the next free block in offset  */
  next_free_block_offset = convert_str_to_int(TFS_buffer, 6, 10);

  // TODO: finish this 

}