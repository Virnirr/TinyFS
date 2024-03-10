#include <time.h>
/* The default size of the disk and file system block */
#define BLOCKSIZE 256 
#define FILENAME_SIZE 9 // including NULL byte
/* Your program should use a 10240 Byte disk size giving you 40 blocks
total. This is a default size. You must be able to support different
possible values */
#define DEFAULT_DISK_SIZE 10240
/* use this name for a default emulated disk file name */
#define DEFAULT_DISK_NAME "tinyFSDisk"

// index
#define NEXT_FREE_BLOCK_IDX 2
#define ROOT_ADDRESS_IDX 1

// block type codes
#define SB_CODE 1
#define INODE_CODE 2
#define FE_CODE 3
#define FB_CODE 4

#define MAGIC_NUM 0x44

// file type codes
#define DIR_CODE 0
#define FILE_CODE 1


#define FILE_DESCRIPTOR_LIMIT 10240
#define PREFIX_SIZE 212
#define FILE_EXTENT_DATA_LIMIT 250

#define NEXT_FREE_BLOCK_OFFSET 

#define DIR_NAME_SIZE 9 /* root + name*/


#define FILE_SIZE_TEMP 5

#define BASE_TEN 10


#define ROOT_POS 1
#define SIZE_OF_INT_IN_STR sizeof(int) + 1
#define SIZE_OF_TIME_T_IN_STR sizeof(time_t) + 1

#define REST_OF_INODE 53
#define REST_OF_SB 246
#define REST_OF_FB 250

/* use as a special type to keep track of files */
typedef int fileDescriptor;

/* stores the superblock content block */
typedef struct __attribute__((__packed__)) superblock {
  char   block_type;               /* byte 0 */
  char   magic_num;                /* byte 1 */
  int    address_of_root;          /* byte 2-5*/
  int    next_free_block;         //  byte 6-9 by having a pointer to the first free block in a chain of free blocks
  char   rest[REST_OF_SB];               /* bytes 10-255 is \0 bytes */
} superblock;


/* stores the inode content block */
typedef struct __attribute__((__packed__)) inode {
  char   block_type;               /* byte 0 */
  char   magic_num;                /* byte 1 */
  char   file_type;                /* byte 2 */
  char   filename[FILENAME_SIZE];  /* byte 3-11 */
  int    file_size;                /* byte 12-15 */
  time_t creation_time;            /* byte 16-23 */
  time_t access_time;              /* byte 24-31 */
  time_t modification_time;        /* byte 32-39 */
  int    first_file_extent;        /* byte 40-43 */
  int    rest[REST_OF_INODE];         /* byte 44-255. Note: Null is index 212 */
} inode;

/* stores the file extent content block */
typedef struct __attribute__((__packed__)) file_extent {
  char   block_type;                 /* byte 0 */
  char   magic_num;                  /* byte 1 */
  int    next_fe;                    /* byte 2-5 */
  char   data[FILE_EXTENT_DATA_LIMIT]; /* byte 6-255 */
} file_extent;

/* stores the free_block content block */
typedef struct __attribute__((__packed__)) free_block {
  char block_type;      /* byte 0 */
  char magic_num;       /* byte 1 */
  int  next_fb;         /* byte 2-5 Note: -1, if end of file */ 
  char rest[REST_OF_FB];        /* byte 6-255 is \0 bytes */
} free_block;

typedef struct __attribute__((__packed__)) file_pointer {
  int  inode_offset;             // check the inode for this file
  int  curr_file_extent_offset;  // current file extent offset
  int  next_file_extent_offset;  // get the next file extent offset
  int  pointer;                  // how mcuh to offset from the current file data
  int  file_size;
} file_pointer;

typedef struct __attribute__((__packed__)) write_node {
  file_extent* curr;
  struct write_node* next;
} write_node;

int tfs_mkfs(char *filename, int nBytes);
/* Makes a blank TinyFS file system of size nBytes on the unix file
specified by ‘filename’. This function should use the emulated disk
library to open the specified unix file, and upon success, format the
file to be a mountable disk. This includes initializing all data to 0x00,
setting magic numbers, initializing and writing the superblock and
inodes, etc. Must return a specified success/error code. */
int tfs_mount(char *diskname);
int tfs_unmount(void);
/* tfs_mount(char *diskname) “mounts” a TinyFS file system located within
‘diskname’. tfs_unmount(void) “unmounts” the currently mounted file
system. As part of the mount operation, tfs_mount should verify the file
system is the correct type. In tinyFS, only one file system may be
mounted at a time. Use tfs_unmount to cleanly unmount the currently
mounted file system. Must return a specified success/error code. */
fileDescriptor tfs_openFile(char *name);
/* Creates or Opens a file for reading and writing on the currently
mounted file system. Creates a dynamic resource table entry for the file,
and returns a file descriptor (integer) that can be used to reference
this entry while the filesystem is mounted. */
int tfs_closeFile(fileDescriptor FD);
/* Closes the file, de-allocates all system resources, and removes table
entry */
int tfs_writeFile(fileDescriptor FD,char *buffer, int size);
/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire
file’s content, to the file system. Previous content (if any) will be 
completely lost. Sets the file pointer to 0 (the start of file) when
done. Returns success/error codes. */
int tfs_deleteFile(fileDescriptor FD);
/* deletes a file and marks its blocks as free on disk. */
int tfs_readByte(fileDescriptor FD, char *buffer);
/* reads one byte from the file and copies it to buffer, using the
current file pointer location and incrementing it by one upon success.
If the file pointer is already past the end of the file then
tfs_readByte() should return an error and not increment the file pointer.
*/
int tfs_seek(fileDescriptor FD, int offset);
/* change the file pointer location to offset (absolute). Returns
success/error codes.*/
