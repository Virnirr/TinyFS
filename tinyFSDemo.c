#include <stdio.h>
#include "libTinyFS.h"
#include <assert.h> // unit test
/* usage void assert(scalar expression); */
#include <string.h>
#include <time.h>
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
  tiny_fs_test_3();
  tiny_fs_test_4();
  tiny_fs_test_5();

  printf("\n--------------------------------------------COMPLETE ALL DEMO------------------------------------------------\n");

  return 0;
}

void tiny_fs_test_1(char *disk_name) {
  /* Testing functionality of making a file, mounting, and unmounting */
  int disk_err;
  char *test_disk_file = "second_disk";
  printf("----------------------- Demo 1: Testing File, Mounting, and Unmounting -----------------------\n\n");

  printf("Demo 1-1: Attempting to create disk: %s\n", disk_name);

  if ((disk_err = tfs_mkfs(disk_name, DEFAULT_DISK_SIZE)) < 0) {
    // this should not fail
    printf("Failed to create TINYFS %s\n", disk_name);
  }
  else {
    printf("Successfully created TINYFS %s\n", disk_name);
  }

  printf("Demo 1-2: Attempting to create disk: %s\n", test_disk_file);

  // creating second disk to test mount
  if ((disk_err = tfs_mkfs(test_disk_file, DEFAULT_DISK_SIZE)) < 0) {
    // this should not fail
    printf("DEMO 1: Failed to create TINYFS %s\n", test_disk_file);
  }
  else {
    printf("DEMO 1: Successfully created TINYFS %s\n", test_disk_file);
  }

  printf("Demo 1-3: Attempting to mount disk: %s\n", disk_name);

  if ((disk_err = tfs_mount(disk_name)) < 0) {
    printf("DEMO: Failed to Mount file. This should not have happened");
  }
  else {
    printf("Demo 1: Successfully Mounted: %s\n", disk_name);
  }

  printf("Demo 1-4: Attempting to unmount: %s\n", disk_name);
  
  if ((disk_err = tfs_unmount()) < 0) {
    printf("DEMO: Failed to unmount. This should not have happened");
  }
  else {
    printf("DEMO 1: Successfully Unmounted: %s\n", disk_name);
  }

  printf("Demo 1-5: Attempting to unmount no files\n");

  if ((disk_err = tfs_unmount()) < 0) {
    printf("DEMO 1: Successfully Failed\n");
  }
  else {
    printf("Failed to fail out of unmount\n");
  }

  printf("Demo 1-6: Attempting to Mount file after another file has mounted\n");

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
  /* Testing Opening, Reading, Writing, Closing, Deleting, and Seeking Files */
  printf("----------------------- DEMO 2: Opening and Deleting Files -----------------------\n\n");
  char *aTestFile = "afile", *bTestFile = "bfile";	/* buffers to store file content */
  char aContent[200] = "Lorem ipsum dolor sit amet consectetur adipiscing elit varius tincidunt nulla pharetra, imperdiet eget lectus class libero vestibulum platea magnis aptent egestas. Euismod natoque dis inceptos imper";
  char bContent[1000] = "Lorem ipsum dolor sit amet consectetur adipiscing elit praesent purus sed, feugiat fames suscipit tincidunt nisi potenti vel porta dignissim venenatis senectus, mauris ut integer at hac netus curabitur rutrum pellentesque. Purus ligula accumsan phasellus mus nostra integer vestibulum, lacinia aptent mauris velit massa lectus, curae per gravida eleifend tincidunt netus. Erat ullamcorper penatibus maecenas lobortis justo massa fames, dignissim cum nibh viverra sollicitudin convallis, tortor ut porttitor faucibus sem egestas. Sociosqu volutpat conubia metus dignissim dictum nulla porta tortor magnis fermentum gravida vulputate curae, class quam curabitur condimentum nostra neque aptent primis urna proin rhoncus. Aenean ornare odio feugiat donec erat sem dignissim senectus natoque vivamus pharetra, non sapien faucibus arcu vehicula eleifend litora etiam mattis ultrices, vestibulum velit augue nascetur mus nunc a nisi blandit tempor. Iaculis potenti class leo porta volutpat tempus id scele.";
  int aFD, bFD;

  printf("DEMO 2-0: Attempting make default disk: %s\n", DEFAULT_DISK_NAME);

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

  printf("DEMO 2-1: Attempting to open file: %s\n", aTestFile);

  if((aFD = tfs_openFile(aTestFile)) < 0) {
    printf("DEMO 2: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 2: SUCCESSFULLY OPENED FILE: %s\n", aTestFile);
  }

  printf("DEMO 2-1: Attempting to open file: %s\n", bTestFile);

  if((bFD = tfs_openFile(bTestFile)) < 0) {
    printf("DEMO 2: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 2: SUCCESSFULLY OPENED FILE: %s\n", bTestFile);
  }
  
  printf("DEMO 2-2: Attempting to write 200 bytes to %s\n", aTestFile);

  if (tfs_writeFile(aFD, aContent, 200) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY WRITTEN 200 bytes into %s\n", aTestFile);
  }

  printf("DEMO 2-2: Attempting to write 1000 bytes to %s\n", bTestFile);

  if (tfs_writeFile(bFD, bContent, 1000) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY WRITTEN 1000 bytes into %s\n", bTestFile);
  }

  printf("DEMO 2-3: Attempting to read 200 bytes from %s\n", aTestFile);


  char readBuffer;
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 2-3: Attempting to read 1000 bytes from %s\n", bTestFile);

  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (bFD, &readBuffer) >= 0) {	/* go until readByte fails */
    printf ("%c", readBuffer);
  }

  fflush(stdout);
  
  printf("\n\n-------------------------------\n\n");

  printf("DEMO 2-3: Attempting to read 500 bytes with tfs_seek from %s\n", bTestFile);

  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  tfs_seek(bFD, 500); // only read 500 bytes
  while (tfs_readByte (bFD, &readBuffer) >= 0) {	/* go until readByte fails */
    printf ("%c", readBuffer);
  }

  fflush(stdout);
  
  printf("\n\n-------------------------------\n\n");

  printf("DEMO 2-4: Attempting to close file %s\n", bTestFile);

  if (tfs_closeFile(aFD) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY CLOSED %s\n", aTestFile);
  }

  printf("DEMO 2-6: Attempting to read from closed file %s\n", aTestFile);

  if (tfs_readByte(aFD, &readBuffer) < 0) {
    printf("DEMO 2: SUCCESSFULLY error after reading closed file: %s\n", aTestFile);
  }
  else {
    printf("THIS SHOULD NOT HAPPEN FROM READ CLOSE FILE\n");
  }

  printf("DEMO 2-7: Attemping to write to closed file %s\n", aTestFile);

  if ((tfs_writeFile(aFD, aContent, 200)) < 0)
	{
	  printf("DEMO 2: SUCCESSFULLY Errored from writing to closed file\n");
	}
  else {
    printf("WRITING TO CLOSED FILE SHOULD NOT REACH HERE\n");
  }

  printf("DEMO 2-8: Attemping to delete file %s\n", bTestFile);

  if ((tfs_deleteFile(bFD)) < 0)
	{
    printf("ERROR OUT FROM delete File");
	}
  else {
	  printf("DEMO 2: SUCCESSFULLY DELETED FILE %s\n", bTestFile);
  }


  printf("DEMO 2 COMPLETE\n\n");

}

void tiny_fs_test_3() {
  /* Testing timestamp, and directory listing and file renaming features  */
  printf("----------------------- DEMO 3: Timestamp, Directory Listing, and Renaming Files -----------------------\n\n");
  char *aTestFile = "afile", *bTestFile = "bfile";	/* buffers to store file content */
  int aFD, bFD;
  time_t aTime, bTime;
  printf("DEMO 3-0: Attempting make default disk: %s\n", DEFAULT_DISK_NAME);

  if(tfs_mkfs (DEFAULT_DISK_NAME, DEFAULT_DISK_SIZE) < 0) {
    perror("tfs_mkds");
  }
  else {
    printf("DEMO 3: SUCCESSFULLY Created Disk: %s\n", DEFAULT_DISK_NAME);
  }

  if (tfs_mount (DEFAULT_DISK_NAME) < 0) {
    perror("tfs_mount");
  }
  else {
    printf("DEMO 3: Successfully mounted %s\n", DEFAULT_DISK_NAME);
  }

  printf("DEMO 3-1: Attempting to open file: %s\n", aTestFile);

  if((aFD = tfs_openFile(aTestFile)) < 0) {
    printf("DEMO 3: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 3: SUCCESSFULLY OPENED FILE: %s\n", aTestFile);
  }
  
  printf("DEMO 3-1: Now printing the creation date of file: %s\n", aTestFile);

  if((aTime = tfs_readFileInfo(aFD)) < 0) {
    printf("DEMO 3: FAILED TO GET CREATION TIME. error code: %d\n", (int) aTime);
  }
  else {
    printf("DEMO 3: CREATION TIME OF %s: %s\n", aTestFile, ctime(&aTime));
  }
  printf("DEMO 3-1: Attempting to open file: %s\n", aTestFile);


  printf("DEMO 3-1: Now sleeping 1 second, then creating another file called bfile and checking the creation date for that\n");
  sleep(1);
  if((bFD = tfs_openFile(bTestFile)) < 0) {
    printf("DEMO 3: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 3: SUCCESSFULLY OPENED FILE: %s\n", bTestFile);
  }
  
  printf("DEMO 3-1: Now printing the creation date of file: %s\n", bTestFile);

  if((bTime = tfs_readFileInfo(bFD)) < 0) {
    printf("DEMO 3: FAILED TO GET CREATION TIME. error code: %d\n", (int) bTime);
  }
  else {
    printf("DEMO 3: CREATION TIME OF %s: %s\n", bTestFile, ctime(&bTime));
  }

  printf("DEMO 3-2: After creating afile and bfile, now listing the directories\n");
  tfs_readdir();
  printf("DEMO 3-3: renaming afile to cfile, now listing the directories\n");
  tfs_rename(aFD, "cfile");
  tfs_readdir();
}

void tiny_fs_test_4() {
  /* Testing read-only and writebyte features  */
  char *aTestFile = "cfile";
  char aContent[200] = "Lorem ipsum dolor sit amet consectetur adipiscing elit varius tincidunt nulla pharetra, imperdiet eget lectus class libero vestibulum platea magnis aptent egestas. Euismod natoque dis inceptos imper";
  char aContent2[5] = "hello";
  int aFD;
  printf("----------------------- DEMO 4: Read-Only files and Writebyte-----------------------\n\n");
  
  printf("DEMO 4-0: List the available files\n");
  tfs_readdir();
  printf("DEMO 4-0: Attempting to open file: %s\n", aTestFile);

  if((aFD = tfs_openFile(aTestFile)) < 0) {
    printf("DEMO 4: FAILED TO OPEN FILE. This should not happen\n");
  }
  else {
    printf("DEMO 4: SUCCESSFULLY OPENED FILE: %s\n", aTestFile);
  }

  printf("DEMO 4-1: Attempting to write 200 bytes to %s (default RW, should work)\n", aTestFile);

  if (tfs_writeFile(aFD, aContent, 200) < 0)
	{
	  perror ("tfs_writeFile failed");
	}
  else {
    printf("DEMO 2: SUCCESSFULLY WRITTEN 200 BYTES INTO %s\n", aTestFile);
  }

  printf("DEMO 4-1: Reading 200 bytes from %s\n", aTestFile);

  char readBuffer;
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 4-2: Setting the pointer to the beginning of the file and writing an 'a'\n");  
  tfs_seek(aFD, SEEK_SET);
  if (tfs_writeByte(aFD, 'a') < 0) {
    perror("DEMO 4: tfs_writeByte failed");
  } else {
    printf("DEMO 4: SUCESSFULLY WROTE 'a' TO BEGINNING OF %s\n", aTestFile);
  }

  printf("DEMO 4-2: Reading 200 bytes from %s\n", aTestFile);

  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 4-3: Writing a 'b' to the 5th byte of the file\n");
  if (tfs_writeByte_offset(aFD, 5, 'b') < 0) {
    perror("DEMO 4: tfs_writeByte_offset failed");
  } else {
    printf("DEMO 4: SUCESSFULLY WROTE 'b' TO THE FIFTH BYTE OF %s\n", aTestFile);
  }

  printf("DEMO 4-3: Reading 200 bytes from %s\n", aTestFile);
  tfs_seek(aFD, SEEK_SET);
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 4-4: Making %s read only\n", aTestFile);
  if (tfs_makeRO(aTestFile) < 0) {
    perror("tfs_makeRO failed");
  }
  else {
    printf("DEMO 4: SUCCESSFULLY CHANGED %s TO READ ONLY\n", aTestFile);
  }

  printf("DEMO 4-4: Attempting to write to %s with tfs_writeFile \n", aTestFile);

  if (tfs_writeFile(aFD, aContent2, 5) < 0)
	{
	  printf("DEMO 4: tfs_writeFile failed\n");
	}
  else {
    printf("DEMO 4: tfs_writeFile success, shouldn't happen\n");
  }

  printf("DEMO 4-4: Attempting to write to %s with tfs_writeByte \n", aTestFile);

  if (tfs_writeByte(aFD, 'a') < 0) {
    printf("DEMO 4: tfs_writeByte failed\n");
  }
  else {
    printf("DEMO 4: tfs_writeByte success, shouldn't happen\n");
  }

  printf("DEMO 4-4: Attempting to write to %s with tfs_writeByte_offset \n", aTestFile);
  if (tfs_writeByte_offset(aFD, 5, 'b') < 0) {
    printf("DEMO 4: tfs_writeByte_offset failed\n");
  } else {
    printf("DEMO 4: tfs_writeByte_offset success, shouldn't happen\n");
  }

  printf("DEMO 4-4: Reading 200 bytes from %s to make sure nothing changed\n", aTestFile);
  tfs_seek(aFD, SEEK_SET);
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");

  printf("DEMO 4-5: Making %s read write again\n", aTestFile);
  if (tfs_makeRW(aTestFile) < 0) {
    perror("tfs_makeRW failed");
  }
  else {
    printf("DEMO 4: SUCCESSFULLY CHANGED %s TO READ WRITE\n", aTestFile);
  }

  printf("DEMO 4-5: Attempting to write to %s with tfs_writeFile \n", aTestFile);

  tfs_seek(aFD, SEEK_SET);
  if (tfs_writeFile(aFD, aContent2, 5) < 0)
	{
	  perror("DEMO 4: tfs_writeFile failed");
	}
  else {
    printf("DEMO 4: Successfully wrote '%s' to the beginning of %s\n", aContent2, aTestFile);
  }

  printf("DEMO 4-5: Attempting to write to %s with tfs_writeByte \n", aTestFile);

  tfs_seek(aFD, SEEK_SET);
  if (tfs_writeByte(aFD, 'h') < 0) {
    perror("DEMO 4: tfs_writeByte failed");
  }
  else {
    printf("DEMO 4: Successfully wrote 'h' to the beginning of %s\n", aTestFile);
  }

  printf("DEMO 4-5: Attempting to write to %s with tfs_writeByte_offset \n", aTestFile);
  if (tfs_writeByte_offset(aFD, 1, 'i') < 0) {
    perror("DEMO 4: tfs_writeByte_offset failed");
  } else {
    printf("DEMO 4: Successfully wrote 'i' to the 1st byte offset of %s\n", aTestFile);
  }

  printf("DEMO 4-5: Reading 200 bytes from %s to make sure writes went through\n", aTestFile);
  tfs_seek(aFD, SEEK_SET);
  printf("-------------------------------\n\n");
  /* now print the rest of it, byte by byte */
  while (tfs_readByte (aFD, &readBuffer) >= 0){	/* go until readByte fails */
    printf ("%c", readBuffer);
  }
  fflush(stdout);

  printf("\n\n-------------------------------\n\n");
  
}

void tiny_fs_test_5() {
  /* Testing mount consistency checks  */
  int disk_fd;
  printf("----------------------- DEMO 5: Mount Consistency Checks-----------------------\n\n");
  printf("DEMO 5-0: Attempting make default disk: %s\n", DEFAULT_DISK_NAME);

  if(tfs_mkfs (DEFAULT_DISK_NAME, DEFAULT_DISK_SIZE) < 0) {
    perror("tfs_mkds");
  }
  else {
    printf("DEMO 5: Successfully created disk: %s\n", DEFAULT_DISK_NAME);
  }

  if (tfs_mount (DEFAULT_DISK_NAME) < 0) {
    perror("tfs_mount");
  }
  else {
    printf("DEMO 5: Successfully mounted %s\n", DEFAULT_DISK_NAME);
  }

  printf("DEMO 5-1: Corrupting the superblock of %s\n", DEFAULT_DISK_NAME);
  // getting disk_fd to corrupt the superblock
  if ((disk_fd = openDisk(DEFAULT_DISK_NAME, 0)) < 0) {
    perror("mount");
  }
  char TFS_buffer[BLOCKSIZE];
  if (write(disk_fd, TFS_buffer, BLOCKSIZE) < 0) {
    perror("write");
  }

  if (readBlock(disk_fd, 0, TFS_buffer) < 0) {
    perror("readBlock");
  }

  printf("DEMO 5: Corrupted superblock\n");
  printf("Block Type: %c\n", ((superblock *)TFS_buffer)->block_type);
  printf("Magic Number: %c\n", ((superblock *)TFS_buffer)->magic_num);
  printf("Address of Root: %d\n", ((superblock *)TFS_buffer)->address_of_root);
  printf("Next Free Block: %d\n", ((superblock *)TFS_buffer)->next_free_block);

  printf("Demo 5-2: Attempting to mount disk with corrupted superblock: %s\n", DEFAULT_DISK_NAME);

  if (tfs_mount(DEFAULT_DISK_NAME) < 0) {
    printf("DEMO 5: Failed to mount file.");
  }
  else {
    printf("DEMO 5: Successfully Mounted: %s, shouldn't happen\n", DEFAULT_DISK_NAME);
  }

  
}