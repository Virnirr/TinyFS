#include <stdio.h>
#include "libTinyFS.h"
#include <assert.h> // unit test
/* usage void assert(scalar expression); */
#include <string.h>
#include "TinyFS_errno.h"

void test_openDisk(char*filename, int nBytes);
void test_mount(char * diskname);
void test_unmount();
void test_openFile_1(char *name);
void test_openFile_2(char *name);
void test_openFile_3(char *name);
int err_num;
int main() {

  printf("Hello World!\n");

  // name to big
  test_openDisk("this_is_too_long_of_name", BLOCKSIZE * 3);

  // nByte too small 
  test_openDisk("filename", 10);

  // file created
  test_openDisk("disk1", BLOCKSIZE * 4);
  test_openDisk("disk2", BLOCKSIZE * 4);
  test_mount("disk1");
  test_mount("disk2");
  test_unmount();

  /* test no disk mounted */
  test_openFile_1("filename");

  // test mount for next filename
  test_mount("disk1");

  /* test open disk with open file > 8 for file name  */
  test_openFile_2("filename2");

  /* test correct filename created and correct output */
  test_openFile_3("filename");

  return 0;
}

void test_mount(char * diskname){
  printf("Checking mount\n");
  if ((err_num = tfs_mount(diskname)) == MOUNT_SUCCESS)
  {
    printf("Mounting %s returned successfully\n", diskname);
  }
  else
  {
    printf("Mounting %s failed, returning error number %d\n", diskname, err_num);
  }
}
void test_unmount(){
  printf("Checking unmount\n");
  if ((err_num = tfs_unmount()) == UNMOUNT_SUCCESS)
  {
    printf("Unmounting returned successfully\n");
  }
  else
  {
    printf("Unmounting failed, returning error number %d\n", err_num);
  }

}
void test_openDisk(char*filename, int nBytes) {
  // too small error
  if ((int) (nBytes / BLOCKSIZE) < 2) {
    printf("Checking a file system created with nBytes too small\n");
    if ((err_num = tfs_mkfs(filename, nBytes)) == EMINLIMIT)
    {
      printf("Successfully detected a filesystem with nBytes too small\n");
    }
    else
    {
      printf("Failed to detect a filesystem with nBytes too small, returning error number %d\n", err_num);
    }
  }

  // name too long error
  else if (strlen(filename) > FILENAME_SIZE) {
    printf("Checking a file system created with string name too large\n");
    if ((err_num = tfs_mkfs(filename, nBytes)) == OPENDISKERROR)
    {
      printf("Successfully detected a filesystem with string name too large\n");
    }
    else
    {
      printf("Failed to detect a filesystem with string name, returning error number %d\n", err_num);
    }
  }

  // make sure that otherwise, it's a valid file descriptor that you opened
  else {
    printf("Checking a file system is valid\n");
    if ((err_num = tfs_mkfs(filename, nBytes)) == 0)
    {
      printf("Successfully created a filesystem\n");
    }
    else
    {
      printf("Failed to create a filesystem, returning error number %d\n", err_num);
    }
  }
}

void test_openFile_1(char *name) {
  printf("Checking a file with no disk mounted returns error\n");
  if ((err_num = tfs_openFile(name)) < 0)
  {
    printf("Successfully detected a file with no disk mounted\n");
  }
  else
  {
    printf("Failed to detected a file with no disk mounted, returning error number %d\n", err_num);
  }
}

void test_openFile_2(char *name) {

  // test file length > 8
  printf("Checking a file with a name too long returns error\n");
  if ((err_num = tfs_openFile(name)) == NOT_A_FILE_SYSTEM)
  {
    printf("Successfully detected a file with a name too long\n");
  }
  else
  {
    printf("Failed to detected a file with a name too long, returning error number %d\n", err_num);
  }
}

void test_openFile_3(char *name) {

  // test mounting to the same disk name
  
  // make sure it's positive (i.e. open file descriptor created)
  printf("Checking if openfile sucessfully creates a file descriptor\n");
  if ((err_num = tfs_openFile(name)) > 0)
  {
    printf("Successfully created a file descriptor for file %s\n", name);
  }
  else
  {
    printf("Failed to create a file descriptor for file %s, returning error number %d\n", name, err_num);
  }
}