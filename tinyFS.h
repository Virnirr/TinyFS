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

/* The default size of the disk and file system block */
#define BLOCKSIZE 256 
#define FILENAME_SIZE 9 // including NULL byte
/* Your program should use a 10240 Byte disk size giving you 40 blocks
total. This is a default size. You must be able to support different
possible values */
#define DEFAULT_DISK_SIZE 10240
/* use this name for a default emulated disk file name */
#define DEFAULT_DISK_NAME “tinyFSDisk”
#define META_DATA_SIZE 4

#define FILE_DESCRIPTOR_LIMIT 10240

/* use as a special type to keep track of files */
typedef int fileDescriptor;

typedef struct file_extent *free_blocks;
// size = 256 bytes
typedef struct Superblock {
  char         meta_data[META_DATA_SIZE];
  free_blocks *free_blocks; // by having a pointer to the first free block in a chain of free blocks
} Superblock;


// keep tracks of meta data of each file in TinyFS
typedef struct inode {
  char meta_data[META_DATA_SIZE];
  char file_name[FILENAME_SIZE];
  int  file_size;
  int  start_idx;
} inode;

typedef struct file_extent {
  // second byte should be 0x44
  char meta_data[META_DATA_SIZE];
  char data[BLOCKSIZE - EXTENET_META_DATA_SIZE];
} file_extent;

