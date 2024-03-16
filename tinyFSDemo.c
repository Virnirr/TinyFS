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


int main() {

  printf(">------------------------------------------------ Demo --------------------------------------------------------<\n\n");

  tiny_fs_test_1();
  tiny_fs_test_2();

  printf("\n>----------------------------------------------Complete Demo---------------------------------------------------<\n");

  return 0;
}

void tiny_fs_test_1() {
  /* Testing functionality of disk */

  printf("------------------------------------------------ Demo 1, Testing Disk ------------------------------------------------\n\n");
  diskTest();
  printf("Demo 1 Complete\n\n");
}


void tiny_fs_test_2() {
  /* Fooad's Test */
  printf("------------------------------------------------ Demo 2: Testing Fooad's ------------------------------------------------\n\n");
  tfsTest();
  tfsTest();
  printf("------------------------------------------------ Demo 2: Complete ------------------------------------------------\n\n");

}

void tiny_fs_test_3() {
  /* Testing functionality for making TinyFS, mount and unmount */

  
  
}

void tiny_fs_test_4() {

  /* Testing opening and closing file functionality */


}

void tiny_fs_test_5() {

  /* Testing default writing and reading (i.e. Fooad's test)*/

  printf("---------------------------- Demo 4: Writing and Reading in TinyFS ----------------------------");

  

  printf("---------------------------- Demo 4: Complete ----------------------------");

}