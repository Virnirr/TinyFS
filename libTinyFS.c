#include "libDisk.h"
#include "tinyFS.h"
#include <sys/stat.h>
#include <string.h>
#include "TinyFS_errno.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MAGIC_IDX 1
#define NEXT_ADDR_IDX 2
#define EMPTY_IDX 3
#define EXIT_FAILURE 1


/*
  Note: table[open_file_idx] = offset_in_tinyFS
  < 0 : File Descriptor not opened.
*/
//when pointer = -1, treat it as null
file_pointer file_descriptor_table[FILE_DESCRIPTOR_LIMIT];

int curr_fs_fd;
int disk_error;
int is_fd_table_unpopulated = 1;

/* ------------------------------------ Util Functions ------------------------------------ */


void fill_new_inode_buffer(inode *inode_buffer, int file_type, char *filename) {
  /* Fills in new inode blocks to write */
  inode_buffer->block_type = INODE_CODE;
  inode_buffer->magic_num = MAGIC_NUM;
  inode_buffer->file_type = file_type;
  if (filename == NULL) {
    strncpy(inode_buffer->filename, "root", FILENAME_SIZE);
  }
  else {
    strncpy(inode_buffer->filename, filename, FILENAME_SIZE);
  }
  inode_buffer->file_size = 0;

  // Get current time as time_t object
  time_t now = time(NULL);
  inode_buffer->creation_time = now;
  inode_buffer->access_time = now;
  inode_buffer->modification_time = now;

  inode_buffer->first_file_extent = -1;

  // set all rest to 0 (i.e. no children)
  memset(inode_buffer->rest, 0, REST_OF_INODE * 4);
}


// Helper function to convert individual bytes (as strings) to an integer, assuming little-endian format

int get_byte_in_int(char *buffer, int start) {
  /* Takes as input */
  int result;
  memcpy(&result, buffer+start, sizeof(int));

  return result;
}

int set_block_to_free(int offset) {
  /* Uses a stack structure LIFO. Util function to set block to free. */
  free_block fe;
  fe.block_type = FE_CODE;
  fe.magic_num = MAGIC_NUM;
  int disk_error;
  int next_free_block_offset;
  char TFS_buffer[BLOCKSIZE];
    // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  /* the next free block in offset  */
  next_free_block_offset = ((superblock *)TFS_buffer)->next_free_block;

  fe.next_fb = next_free_block_offset; // set it to first
  
    // write content into disk
  if (writeBlock(curr_fs_fd, offset, &fe) < 0) {
    perror("Disk Write");
    return -1;
  }

  // making the block u just freed into the next_free_block of superblock
  memcpy(&((superblock *)TFS_buffer)->next_free_block, &offset, sizeof(int));
  if (writeBlock(curr_fs_fd, 0, TFS_buffer) < 0) {
  perror("Disk Write");
  return -1;
  }
  return 0;
}

int remove_next_free_and_set_free_after_it() {
  /* get next free block and set it to the free block after it*/
  char TFS_buffer[BLOCKSIZE];
  int next_free_block_offset, free_block_after_it;
  // get the superblock check for first free block
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }
  /* the next free block in offset  */
  next_free_block_offset = get_byte_in_int(TFS_buffer, 6);
  if(next_free_block_offset == -1){
    return LIMIT_REACHED;
  } 
  // TODO: finish this 
  //read the next_free_block, then get it's free block after it
  if ((disk_error = readBlock(curr_fs_fd, next_free_block_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  free_block_after_it = get_byte_in_int(TFS_buffer, 2);

  // printf("next_free_block_offset: %d\n", next_free_block_offset);

  
  // get the superblock again
  if ((disk_error = readBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }
  //converting free block after it into char bytes
  ((superblock *)TFS_buffer)->next_free_block = free_block_after_it;

  //writing the buffer back into superblock
  if ((disk_error = writeBlock(curr_fs_fd, 0, TFS_buffer)) < 0) {
    return disk_error;
  }

  return next_free_block_offset;
}

/* ------------------------------------ Main TinyFS library functions ------------------------------------ */

int tfs_mkfs(char *filename, int nBytes) {
  /*
    Makes a blank TinyFS file system of size nBytes and mount it onto
    the unix "filename" file. 
  */
  // populates fd table
  for(int i = 0; i < FILE_DESCRIPTOR_LIMIT; i++){
    file_descriptor_table[i].inode_offset = -1;
    file_descriptor_table[i].curr_file_extent_offset = -1;
    file_descriptor_table[i].next_file_extent_offset = -1;
    file_descriptor_table[i].pointer = -1;
    file_descriptor_table[i].file_size = -1;
  }
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

  // initialize the rest as 0
  for (int i = 0; i < REST_OF_SB; i++)
  {
    sb.rest[i] = '\0';
  }


  // open new filename
  int disk_fd = curr_fs_fd = openDisk(filename, nBytes);
  
  if (disk_fd < 0) {
    return -1;
  }


  char buffer[BLOCKSIZE];
  // write superblock into first block of file system
  if ((disk_error = writeBlock(disk_fd, 0, &sb)) < 0) {
    perror("write block");
    return disk_error;
  }

    // write superblock into first block of file system
  if ((disk_error = readBlock(disk_fd, 0, buffer)) < 0) {
    perror("write block");
    return disk_error;
  }



  // write the root inode into the second block in TinyFS
  inode root;
  fill_new_inode_buffer(&root, DIR_CODE, NULL); // only root will have filename = NULL
  
  // even though in sb you have address_of_root = 1
  int fs_idx = ROOT_POS; // 1
  
  // write the root block into TinyFS
  if ((disk_error = writeBlock(disk_fd, fs_idx, &root)) < 0) {
    perror("write block");
    return disk_error;
  }


  
  // initialize free block
  free_block fb;
  fb.block_type = FB_CODE;
  fb.magic_num = MAGIC_NUM;

  // initialize the rest as 0
  for (int i = 0; i < REST_OF_FB; i++)
  {
    fb.rest[i] = '\0';
  }

  // fill the rest of the memory as free blocks
  for (fs_idx = 2; fs_idx < (int) (nBytes / BLOCKSIZE) - 1; fs_idx++) {
    // initialize next free block as linked list and write it into file system
    fb.next_fb = fs_idx + 1;
    if ((disk_error = writeBlock(disk_fd, fs_idx, &fb)) < 0) {
      perror("write block");
      return disk_error;
    }
  }
  
  if ((int) (nBytes / BLOCKSIZE) > 2) {
    // last free block points to -1
    fb.next_fb = -1;
    if ((disk_error = writeBlock(disk_fd, fs_idx, &fb)) < 0) {
      perror("write block");
      return disk_error;
    }
  }

  
  // return 0 on success
  return 0;
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

//“mounts” a TinyFS file system located within ‘diskname’
int tfs_mount(char *diskname) {
  int disk_fd;
  // resets the fd table
  for(int i = 0; i < FILE_DESCRIPTOR_LIMIT; i++){
    file_descriptor_table[i].inode_offset = -1;
    file_descriptor_table[i].curr_file_extent_offset = -1;
    file_descriptor_table[i].next_file_extent_offset = -1;
    file_descriptor_table[i].pointer = -1;
    file_descriptor_table[i].file_size = -1;
  }

  // open existing disk
  if ((disk_fd = openDisk(diskname, 0)) < 0) {
    perror("mount");
    return disk_fd; // this will be the error number
  }

  // make sure that every block is valid TinyFS using magic number check
  int tinyfs_offset_idx;
  struct stat buf;
  char TFS_buffer[BLOCKSIZE];

  if (fstat(disk_fd, &buf) < 0) {
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
  if ((status = tfs_unmount() < 0))
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

fileDescriptor tfs_openFile(char *name) {

  // unsigned long fd_idx = hash(name) % FILE_DESCRIPTOR_LIMIT;
  int fd_table_idx;
  file_pointer curr_fd_table_pointer;
  int logical_offset;
  int first_null_fd_idx = -1;

  char TFS_buffer[BLOCKSIZE];
  char filename_buffer[FILENAME_SIZE];

  if (curr_fs_fd < 0) {
    return NO_DISK;
  }
  
  // ? this is file name, not of the file system, change error
  if (!name || strlen(name) > FILENAME_SIZE - 1) {
    return NOT_A_FILE_SYSTEM;
  }

  // check all the filedescriptor table to see if you can find the filename containing "name"
  for (fd_table_idx = 0; fd_table_idx < FILE_DESCRIPTOR_LIMIT; fd_table_idx++) {
    // if the fd_table_idx is not NULL, check to see if the inode filename matches parameter name.
    // if it does, return that file descriptor since it's already opened.

    // // save the first null file descriptor table index for later when creating new file
    // // so you don't have to loop twice.
    if (first_null_fd_idx == -1 && file_descriptor_table[fd_table_idx].pointer == -1) {
      first_null_fd_idx = fd_table_idx;
    }

    // if the fd_table_idx 's pointer != -1, check if it's the opened file is "name"
    if (file_descriptor_table[fd_table_idx].pointer != -1) {

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

  int num_read;
  /* read from start */
  if (lseek(curr_fs_fd, 0, SEEK_SET) == -1) {
      perror("lseek");
      exit(EXIT_FAILURE);
  }

  char buffer[BLOCKSIZE];
  int logical_disk_offset = 0;
  file_pointer fp;

  // find it with linear probing (assuming it's not a directory)
  while ((num_read = read(curr_fs_fd, buffer, BLOCKSIZE)) > 0) {
    // if it's an inode and the name matches, then you cache it in open directory file
    if (((inode *)buffer)->block_type == INODE_CODE && 
        !strcmp(((inode *)buffer)->filename, name)) {
      // find a location that exist and set that location as the logical disk offset.
      // file_fd_idx = hash(name); do linear probe instead
      // while (file_descriptor_table[file_fd_idx].pointer != -1) {
      //   file_fd_idx++;
      // }
      // initialize file pointer and store into file descriptor table
      fp.inode_offset = logical_disk_offset;
      
      // get the file extent and store into file pointer from the buffer
      fp.curr_file_extent_offset = ((inode *)buffer)->first_file_extent;
      fp.pointer = 0; // where the first data is pointed at in the file extent
      fp.file_size = ((inode *)buffer)->file_size;
      fp.next_file_extent_offset = -1; // default for next file extent

      // read the first file extent if there is any and get the next file extent offset from it
      if (fp.curr_file_extent_offset != -1) {
        if (readBlock(curr_fs_fd, fp.curr_file_extent_offset, buffer) < 0) {
          return -1;
        }
        fp.next_file_extent_offset = ((file_extent *)buffer)->next_fe;
      }

      // store fp into global file descriptor table
      file_descriptor_table[first_null_fd_idx] = fp;
      return first_null_fd_idx;
    }
    logical_disk_offset++;
  }
  if (num_read < 0) {
    perror("read");
    return -1;
  }

  // fill new_inode if it's not found
  int first_free_space;
  inode new_file_inode;
  // if it's a new name, then create a new inode and place it onto the first free space
  if (file_descriptor_table[first_null_fd_idx].pointer == -1) {
    // printf("came here to fill new file\n");
    first_free_space = remove_next_free_and_set_free_after_it();
    // fill new_inode if it's not found
    fill_new_inode_buffer(&new_file_inode, FILE_CODE, name);
    
    if (writeBlock(curr_fs_fd, first_free_space, &new_file_inode) < 0) {
      return EOPENFILE;
    }
    fp.inode_offset = first_free_space;
    fp.pointer = 0; // where the first data is pointed at in the file extent
    fp.file_size = 0; // 0, since creating a new file
    fp.next_file_extent_offset = -1; // default for next file extent
    fp.curr_file_extent_offset = -1; // default for current file extent

    // printf("%d\n", fp.inode_offset);

    file_descriptor_table[first_null_fd_idx] = fp;
  }

  // printf("this is the inode: %d\n",fp.inode_offset);
  return first_null_fd_idx;
}

int tfs_closeFile(fileDescriptor FD) {
  /* Closes the file, de-allocates all system resources, 
     and removes table entry) */
  
  if (file_descriptor_table[FD].pointer == -1) {
    return EBADF;
  }

  // remove entry from fd table
  file_descriptor_table[FD].inode_offset = -1;
  file_descriptor_table[FD].curr_file_extent_offset = -1;
  file_descriptor_table[FD].next_file_extent_offset = -1;
  file_descriptor_table[FD].pointer = -1;
  file_descriptor_table[FD].file_size = -1;

  return 0;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
  /* Writes buffer ‘buffer’ of size ‘size’, which represents an entire
file’s content, to the file system.  */
  
  //char size_temp[FILE_SIZE_TEMP] = {'\0'};
  //char first_file_extent[FILE_SIZE_TEMP] = {'\0'};
  // int file_size;
  int file_content_offset, disk_error,next_file_content_offset;
  char curr_filename[FILENAME_SIZE];

  // internal buffer to store the read block
  char TFS_buffer[BLOCKSIZE];

  file_pointer file_pointer = file_descriptor_table[FD];

  // check if valid fd
  if (file_pointer.pointer == -1) {
    perror("unopened file");
    return EBADF;
  }
  
  // get the inode of file to check next file extent
  if ((disk_error = readBlock(curr_fs_fd, file_pointer.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }

  file_content_offset = ((inode *)TFS_buffer)->first_file_extent;

  // copy to the current filename buffer
  strcpy(curr_filename, ((inode *)TFS_buffer)->filename);
  


  // free the current content so you restart over
  while (file_content_offset != -1) {

    // get file content of the file descriptor and initialize the values of TFS_buffer with the given block
    if ((disk_error = readBlock(curr_fs_fd, file_content_offset, TFS_buffer)) < 0) {
      return disk_error;
    }
    // set it to next content offset (if there is any) so you can free that one too
    next_file_content_offset = get_byte_in_int(TFS_buffer, 2);
    if (set_block_to_free(file_content_offset) < 0) {
      perror("set block to free");
      return -1;
    }
    // will become -1 at the end when you have no next files
    file_content_offset = next_file_content_offset;
  }

  // write and save the first file extent to the inode
  int write_size = size;
  int free_block_offset = remove_next_free_and_set_free_after_it();

  if (free_block_offset == -1) {
    return LIMIT_REACHED;
  }

  // updating the inode values
  ((inode *)TFS_buffer)->file_size = size;
  ((inode *)TFS_buffer)->modification_time = time(NULL);
  ((inode *)TFS_buffer)->first_file_extent = free_block_offset;

  // write inode to disk
  if (writeBlock(curr_fs_fd, file_pointer.inode_offset, TFS_buffer) < 0) {
    perror("Disk Write");
    return -1;
  }

  file_extent *head_fe;
  file_extent *curr_fe;
  file_extent *prev_fe;

  int curr_offset = free_block_offset;

  // sys call failure so exit
  if ((head_fe = (file_extent *)calloc(1, sizeof(file_extent))) == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  head_fe -> block_type = FE_CODE;
  head_fe -> magic_num = MAGIC_NUM;
  head_fe -> next_fe = -1;

  // set prev_fe initially to head
  prev_fe = head_fe;
  curr_fe = head_fe;
  
  write_node *head_write_node; // keeps the head write_node
  write_node *curr_write_node; // keeps the current write_node
  //write_node *next_write_node; // keeps the next write_node
  write_node * temp_write_node; //idk
  if ((curr_write_node = (write_node *)malloc(sizeof(write_node))) < 0) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  curr_write_node -> curr = head_fe;
  curr_write_node -> next = NULL;

  // head set to the first write_node
  head_write_node = curr_write_node;

  // copy up to FILE_EXTENT_DATA_LIMIT into fe.data
  strncpy(head_fe -> data, buffer, FILE_EXTENT_DATA_LIMIT);

  // subtract from total to keep track of how much write
  write_size -= FILE_EXTENT_DATA_LIMIT;

  // go to the next set of bytes to write
  buffer += FILE_EXTENT_DATA_LIMIT;

  // If there is still more, add it to the end of linked list of file extent
  while (write_size > 0) {

    // sys call failure so exit 
    if ((curr_fe = (file_extent *)calloc(1, sizeof(file_extent))) == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }
    
    curr_fe -> block_type = FE_CODE;
    curr_fe -> magic_num = MAGIC_NUM;
    curr_fe -> next_fe = -1;

    // get new free file extente
    free_block_offset = remove_next_free_and_set_free_after_it();

    if (free_block_offset < 0) {
      return LIMIT_REACHED;
    }

    // set previous file extent pointer to point to new one
    prev_fe -> next_fe = free_block_offset;

    // set previous file extent pointer to the current
    prev_fe = curr_fe;

    // create a linked list of write nodes for later
    if ((temp_write_node = (write_node *)malloc(sizeof(write_node))) < 0) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    // temp is current node being added
    temp_write_node -> curr = curr_fe;
    temp_write_node -> next = NULL;

    // set the current node to temp and set it as temp for next loop
    curr_write_node -> next = temp_write_node;
    curr_write_node = temp_write_node; 

    // copy up to FILE_EXTENT_DATA_LIMIT into fe.data
    strncpy(curr_fe -> data, buffer, FILE_EXTENT_DATA_LIMIT);

    // subtract from total to keep track of how much write
    write_size -= FILE_EXTENT_DATA_LIMIT;

    // go to the next set of bytes to write
    buffer += FILE_EXTENT_DATA_LIMIT;
  }
  
  write_node *next_write_node;
  curr_write_node = head_write_node;
  
  // while head_write_node is not NULL, write all block and free everything
  while (curr_write_node) {
    
    // write content into disk
    if (writeBlock(curr_fs_fd, curr_offset, curr_write_node->curr) < 0) {
      perror("Disk Write");
      return -1;
    }

    curr_offset = curr_write_node->curr->next_fe;

    // free everything
    next_write_node = curr_write_node->next;
    free(curr_write_node->curr);
    free(curr_write_node);
    curr_write_node = next_write_node;
  }

  return 0;
}

int tfs_deleteFile(fileDescriptor FD) {
  /* Takes a file descriptor and remove the file (set it as free block) in the tinyFS */

  file_pointer fp_struct = file_descriptor_table[FD];
  
  char TFS_buffer[BLOCKSIZE];
  int disk_error;
  // get the superblock check for first file content block
  if ((disk_error = readBlock(curr_fs_fd, fp_struct.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  //gets content of inode
  int file_content_offset = ((inode *)TFS_buffer)->first_file_extent;
  int next_content_offset;

  // there's content, so set it to free block
  while (file_content_offset != -1) {
    // get the superblock check for first free block
    if ((disk_error = readBlock(curr_fs_fd, file_content_offset, TFS_buffer)) < 0) {
      return disk_error;
    }
    // set it to next content offset (if there is any) so you can free that one too
    next_content_offset = ((file_extent *)TFS_buffer)->next_fe;
    if (set_block_to_free(file_content_offset) < 0) {
      perror("set block to free");
      return -1;
    }
    file_content_offset = next_content_offset;
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
  int pointer = curr_file_pointer.pointer % FILE_EXTENT_DATA_LIMIT;
  int next_fe_offset = curr_file_pointer.next_file_extent_offset;
  int curr_fe_offset = curr_file_pointer.curr_file_extent_offset;

  // File pointer has already past the end of the file
  if (curr_file_pointer.pointer >= curr_file_pointer.file_size ) {
    perror("end of file");
    return TFS_EFO;
  }

  // buffer that stores the file extent contents
  char TFS_buffer[BLOCKSIZE];

  // end of data block for file pointer and there's still a data block, 
  // change curr_file_extent_offset and next_file_extent_offset
  if (curr_file_pointer.pointer >= FILE_EXTENT_DATA_LIMIT) {
    int num_next = curr_file_pointer.pointer / FILE_EXTENT_DATA_LIMIT;
    for (int i = 0; i < num_next; i++) {
      if (next_fe_offset != -1) {
        int disk_error;
          if ((disk_error = readBlock(curr_fs_fd, next_fe_offset, TFS_buffer)) < 0) {
            return disk_error;
          }
        curr_fe_offset = next_fe_offset;
        next_fe_offset = ((file_extent *)TFS_buffer)->next_fe;
      }
    }
  }

  // read in the current file extent block
  if ((disk_error = readBlock(curr_fs_fd, curr_fe_offset, TFS_buffer)) < 0) {
    return disk_error;
  }

  // copy the one byte content into buffer
  strncpy(buffer, ((file_extent *)TFS_buffer)->data + pointer, 1);


  // increment current file pointer to the next byte and update in fd table
  curr_file_pointer.pointer++;
  file_descriptor_table[FD].pointer++;

  // update the access_time in the inode
  if ((disk_error = readBlock(curr_fs_fd, curr_file_pointer.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  ((inode *)TFS_buffer)->access_time = time(NULL);
  if (writeBlock(curr_fs_fd, curr_file_pointer.inode_offset, TFS_buffer) < 0) {
    perror("Disk Write");
    return -1;
  }
  return 0;
}

// change the file pointer location to offset (absolute). 
// Returns success/error codes
int tfs_seek(fileDescriptor FD, int offset) {
  /* implement seek based off of tfs_readByte */

  file_pointer curr_file_pointer = file_descriptor_table[FD];
  // if fd doesn't exist
  if (curr_file_pointer.pointer == -1)
  {
    perror("unopened file");
    return EBADF;
  }
  // if offset is invalid
  if (offset > curr_file_pointer.file_size || offset < 0){
    perror("offset");
    return OFFSET_FAIL;
  }

  // int pointer = curr_file_pointer.pointer;
  // pointer = offset % FILE_EXTENT_DATA_LIMIT;
  //get curr_file_extent_offset
 
  char TFS_buffer[BLOCKSIZE];
  // initially sets curr_file_extent_offset to the first extent
  // and next_file_extent_offset to the one after first extent
  int read_inode;
  if ((read_inode = readBlock(FD, curr_file_pointer.inode_offset, TFS_buffer)) < 0) {
      return read_inode;
  }
  int curr_file_extent = get_byte_in_int(TFS_buffer, 40);
  if ((read_inode = readBlock(FD, curr_file_extent, TFS_buffer)) < 0) {
      return read_inode;
  }
  int next_file_extent = get_byte_in_int(TFS_buffer, 2);
  // then, if offset is larger than one file extent, loop through the file extents 
   int page_offset = (int) (offset / FILE_EXTENT_DATA_LIMIT);
  for (int i = 0; i < page_offset; i++)
  {
    curr_file_extent = next_file_extent;
    if ((read_inode = readBlock(FD, curr_file_extent, TFS_buffer)) < 0) {
    return read_inode;
    }
    next_file_extent = get_byte_in_int(TFS_buffer, 2);
  }
  return 0;
}

//returns a file descriptor's creation time
time_t tfs_readFileInfo(fileDescriptor FD) {
  file_pointer fd_file_pointer = file_descriptor_table[FD];
// if fd doesn't exist
  if (fd_file_pointer.pointer == -1)
  {
    perror("unopened file");
    return EBADF;
  }
  //grabbing inode buffer   
  char TFS_buffer[BLOCKSIZE];
  if ((disk_error = readBlock(FD, fd_file_pointer.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  //reading the creation time at it's location in inode 
  char time_in_str[SIZE_OF_TIME_T_IN_STR];
  strncpy(time_in_str, TFS_buffer+16, SIZE_OF_INT_IN_STR - 1);
  time_in_str[SIZE_OF_INT_IN_STR] = '\0';
  // changing accessed time to now, and writing it back
  time_t now = time(NULL);
  memcpy(TFS_buffer + 24, &now, sizeof(time_t));
  if ((disk_error = writeBlock(FD, fd_file_pointer.inode_offset, TFS_buffer)) < 0) {
    return disk_error;
  }
  //returning creation time
  return (time_t) strtol(time_in_str, NULL, BASE_TEN);
}
