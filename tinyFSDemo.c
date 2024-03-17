#include <stdio.h>
#include "libTinyFS.h"
#include <assert.h> // unit test
/* usage void assert(scalar expression); */
#include <string.h>
#include "TinyFS_errno.h"
#include "libDisk.h"

int err_num;

void tiny_fs_test_1();
void tiny_fs_test_2();
void tiny_fs_test_3();
void tiny_fs_test_4();
void tiny_fs_test_5();

/* simple helper function to fill Buffer with as many inPhrase strings as possible before reaching size */
int
fillBufferWithPhrase (char *inPhrase, char *Buffer, int size)
{
  int index = 0, i;
  if (!inPhrase || !Buffer || size <= 0 || size < strlen (inPhrase))
    return -1;

  while (index < size)
    {
      for (i = 0; inPhrase[i] != '\0' && (i + index < size); i++)
	Buffer[i + index] = inPhrase[i];
      index += i;
    }
  Buffer[size - 1] = '\0';	/* explicit null termination */
  return 0;
}


int main() {

  char diskName[] = "diskX.dsk"; 

  printf("------------------------------------------------ DEMO --------------------------------------------------------\n\n");

  tiny_fs_test_1(diskName);
  tiny_fs_test_2();

  printf("\n--------------------------------------------COMPLETE ALL DEMO------------------------------------------------\n");

  return 0;
}

void tiny_fs_test_1(char *disk_name) {
  /* Testing functionality of making a file, mounting, and unmounting */
  int disk_err;
  char *test_disk_file = "second_disk";
  printf("----------------------- Demo 1: Testing File, Mounting, and Unmounting -----------------------\n\n");

  printf("Demo 1-1: Attemping to creating file: %s\n", disk_name);

  if ((disk_err = tfs_mkfs(disk_name, DEFAULT_DISK_SIZE)) < 0) {
    // this should not fail
    printf("Failed to create TINYFS %s with error code '%d'\n", disk_name, disk_err);
  }
  else {
    printf("Successfully created TINYFS %s with error code '%d'\n", disk_name, disk_err);
  }

  printf("Demo 1-2: Attemping to creating file: %s\n", test_disk_file);

  // creating second disk to test mount and unmount
  if ((disk_err = tfs_mkfs(test_disk_file, DEFAULT_DISK_SIZE)) < 0) {
    // this should not fail
    printf("DEMO 1: Failed to create TINYFS %s with error code '%d'\n", disk_name, disk_err);
  }
  else {
    printf("DEMO 1: Successfully created TINYFS %s with error code '%d'\n", disk_name, disk_err);
  }

  printf("Demo 1-3: Attemping to mount file: %s\n", disk_name);

  if ((disk_err = tfs_mount(disk_name)) < 0) {
    printf("DEMO: Failed to Mount file. This should not have happened");
  }
  else {
    printf("Demo 1: Successfully Mounted: %s\n", disk_name);
  }

  printf("Demo 1-4: Attemping to unmount file: %s\n", disk_name);
  
  if ((disk_err = tfs_unmount()) < 0) {
    printf("DEMO: Failed to unmount file. This should not have happened");
  }
  else {
    printf("DEMO 1: Successfully Unmounted: %s\n", disk_name);
  }

  printf("Demo 1-5: Attemping to unmount no files\n");

  if ((disk_err = tfs_unmount()) < 0) {
    printf("DEMO 1: Successfully Failed\n");
  }
  else {
    printf("Failed to fail out of unmount\n");
  }

  printf("Demo 1-6: Attemping to Mount file after another file\n");

  if ((disk_err = tfs_mount(disk_name)) < 0) {
    printf("DEMO: Failed to Mount file. This should not have happened");
  }
  else {
    printf("DEMO 1: Successfully Mounted: %s\n", disk_name);
  }

  if ((disk_err = tfs_mount(test_disk_file)) < 0) {
    printf("DEMO: Failed to Mount file. This should not have happened");
  }
  else {
    printf("DEMO 1: Successfully Mounted another file: %s\n", test_disk_file);
  }

  printf("DEMO 1 COMPLETE\n\n");
}


void tiny_fs_test_2() {
  /* Testing Opening, Reading, Writing, Deleting, and Seeking Files */
  printf("----------------------- DEMO 2: Opening and Deleting Files -----------------------\n\n");
  char *aTestFile = "afile", *bTestFile = "bfile";	/* buffers to store file content */
  char aContent[200] = "Lorem ipsum dolor sit amet consectetur adipiscing elit varius tincidunt nulla pharetra, imperdiet eget lectus class libero vestibulum platea magnis aptent egestas. Euismod natoque dis inceptos imper";
  char bContent[1000] = "Lorem ipsum dolor sit amet consectetur adipiscing elit praesent purus sed, feugiat fames suscipit tincidunt nisi potenti vel porta dignissim venenatis senectus, mauris ut integer at hac netus curabitur rutrum pellentesque. Purus ligula accumsan phasellus mus nostra integer vestibulum, lacinia aptent mauris velit massa lectus, curae per gravida eleifend tincidunt netus. Erat ullamcorper penatibus maecenas lobortis justo massa fames, dignissim cum nibh viverra sollicitudin convallis, tortor ut porttitor faucibus sem egestas. Sociosqu volutpat conubia metus dignissim dictum nulla porta tortor magnis fermentum gravida vulputate curae, class quam curabitur condimentum nostra neque aptent primis urna proin rhoncus. Aenean ornare odio feugiat donec erat sem dignissim senectus natoque vivamus pharetra, non sapien faucibus arcu vehicula eleifend litora etiam mattis ultrices, vestibulum velit augue nascetur mus nunc a nisi blandit tempor. Iaculis potenti class leo porta volutpat tempus id scele.";
  int aFD, bFD;

  printf("DEMO 2-0: Attemping make default disk: %s\n", DEFAULT_DISK_NAME);

  if(tfs_mkfs (DEFAULT_DISK_NAME, DEFAULT_DISK_SIZE) < 0) {
    perror("tfs_mkds");
  }
  else {
    printf("DEMO 2: SUCCESSFULLY Created Disk: %s\n", DEFAULT_DISK_NAME);
  }

  if (tfs_mount (DEFAULT_DISK_NAME) < 0) {
    perror("tfs_mount");
  }
  else {
    printf("DEMO 2: Successfully mounted %s\n", DEFAULT_DISK_NAME);
  }

  printf("DEMO 2-1: Attemping to open file: %s\n", aTestFile);

  if((aFD = tfs_openFile(aTestFile)) < 0) {
    printf("DEMO 2: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 2: SUCCESSFULLY OPENED FILE: %s\n", aTestFile);
  }

  printf("DEMO 2-1: Attemping to open file: %s\n", bTestFile);

  if((bFD = tfs_openFile(bTestFile)) < 0) {
    printf("DEMO 2: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 2: SUCCESSFULLY OPENED FILE: %s\n", bTestFile);
  }
  
  printf("DEMO 2-2: Attemping to write 200 bytes to %s\n", aTestFile);

  if (tfs_writeFile(aFD, aContent, 200) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY WRITTEN 200 bytes into %s\n", aTestFile);
  }

  printf("DEMO 2-2: Attemping to write 1000 bytes to %s\n", bTestFile);

  if (tfs_writeFile(bFD, bContent, 1000) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY WRITTEN 1000 bytes into %s\n", bTestFile);
  }

  printf("DEMO 2-3: Attemping to read 200 bytes from %s\n", aTestFile);


  char readBuffer;
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 2-3: Attemping to read 1000 bytes from %s\n", bTestFile);

  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (bFD, &readBuffer) >= 0) {	/* go until readByte fails */
    printf ("%c", readBuffer);
  }

  fflush(stdout);
  
  printf("\n\n-------------------------------\n\n");

  printf("DEMO 2-3: Attemping to read 500 bytes with tfs_seek from %s\n", bTestFile);

  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  tfs_seek(bFD, 500); // only read 500 bytes
  while (tfs_readByte (bFD, &readBuffer) >= 0) {	/* go until readByte fails */
    printf ("%c", readBuffer);
  }

  fflush(stdout);
  
  printf("\n\n-------------------------------\n\n");


  printf("DEMO 2 COMPLETE\n\n");

}

void tiny_fs_test_3() {
  /* Testing reading and writing to files */

  
  
}

void tiny_fs_test_4() {

  /* Testing opening and closing file functionality */


}

void tiny_fs_test_5() {

  /* Testing default writing and reading (i.e. Fooad's test)*/

  printf("---------------------------- Demo 4: Writing and Reading in TinyFS ----------------------------");

  

  printf("---------------------------- Demo 4: Complete ----------------------------");

}