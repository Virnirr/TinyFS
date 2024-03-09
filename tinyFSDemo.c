#include <stdio.h>
#include "tinyFS.h"
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
  assert(tfs_mount(diskname) == MOUNT_SUCCESS);
}
void test_unmount(){
  assert(tfs_unmount() == UNMOUNT_SUCCESS);
}
void test_openDisk(char*filename, int nBytes) {
  // too small error
  if ((int) (nBytes / BLOCKSIZE) < 2) {
    assert(tfs_mkfs(filename, nBytes) == EMINLIMIT);
  }

  // name too long error
  else if (strlen(filename) > FILENAME_SIZE) {
    assert(tfs_mkfs(filename, nBytes) == OPENDISKERROR);
  }

  // make sure that otherwise, it's a valid file descriptor that you opened
  else {
    assert(tfs_mkfs(filename, nBytes) == 0);
  }
}

void test_openFile_1(char *name) {
  assert(tfs_openFile(name) < 0);
}

void test_openFile_2(char *name) {

  // test file length > 8
  assert(tfs_openFile(name) == NOT_A_FILE_SYSTEM);

}

void test_openFile_3(char *name) {

  // test mounting to the same disk name
  
  // make sure it's positive (i.e. open file descriptor created)
  assert(tfs_openFile(name) > 0);
}