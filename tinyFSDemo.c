#include <stdio.h>
#include "tinyFS.h"
#include <assert.h> // unit test
/* usage void assert(scalar expression); */
#include <string.h>
#include "TinyFS_errno.h"

void test_openDisk(char*filename, int nBytes);

int main() {

  printf("Hello World!\n");

  // name to big
  test_openDisk("this_is_too_long_of_name", BLOCKSIZE * 3);

  // nByte too small 
  test_openDisk("filename", 10);

  // file created
  test_openDisk("filename", BLOCKSIZE * 2);

  return 0;
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