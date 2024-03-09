Name: Zhixiang He, Sean Phun, Dennis Phun

TinyFS is a very simple file system. It is purposefully under-specified, giving you the freedom to
implement it using many of the algorithms and primitives you’ve learned throughout this course.
TinyFS does not support a hierarchical namespace, i.e. there are no directories beyond the root
directory, and all the files are in a flat namespace. 

# Get how big a filesize is:
```console
stat -c %s <path>
```

# How to test libDisk.c

## Step 1:
Create diskTest executable
```
make test
```

## Step 2:
Run test
```
./diskTest
```


Correct Output:
```
open disk with 0 bytes: No such file or directory
] Open failed with (-1). Disk probably does not exist.
] Successfully wrote to block 25 of disk disk0.dsk.
] Successfully wrote to block 39 of disk disk0.dsk.
] Successfully wrote to block 8 of disk disk0.dsk.
] Successfully wrote to block 9 of disk disk0.dsk.
] Successfully wrote to block 15 of disk disk0.dsk.
] Successfully wrote to block 21 of disk disk0.dsk.
] Successfully wrote to block 25 of disk disk0.dsk.
] Successfully wrote to block 33 of disk disk0.dsk.
] Successfully wrote to block 35 of disk disk0.dsk.
] Successfully wrote to block 42 of disk disk0.dsk.
open disk with 0 bytes: No such file or directory
] Open failed with (-1). Disk probably does not exist.
] Successfully wrote to block 25 of disk disk1.dsk.
] Successfully wrote to block 39 of disk disk1.dsk.
] Successfully wrote to block 8 of disk disk1.dsk.
] Successfully wrote to block 9 of disk disk1.dsk.
] Successfully wrote to block 15 of disk disk1.dsk.
] Successfully wrote to block 21 of disk disk1.dsk.
] Successfully wrote to block 25 of disk disk1.dsk.
] Successfully wrote to block 33 of disk disk1.dsk.
] Successfully wrote to block 35 of disk disk1.dsk.
] Successfully wrote to block 42 of disk disk1.dsk.
open disk with 0 bytes: No such file or directory
] Open failed with (-1). Disk probably does not exist.
] Successfully wrote to block 25 of disk disk2.dsk.
] Successfully wrote to block 39 of disk disk2.dsk.
] Successfully wrote to block 8 of disk disk2.dsk.
] Successfully wrote to block 9 of disk disk2.dsk.
] Successfully wrote to block 15 of disk disk2.dsk.
] Successfully wrote to block 21 of disk disk2.dsk.
] Successfully wrote to block 25 of disk disk2.dsk.
] Successfully wrote to block 33 of disk disk2.dsk.
] Successfully wrote to block 35 of disk disk2.dsk.
] Successfully wrote to block 42 of disk disk2.dsk.
open disk with 0 bytes: No such file or directory
] Open failed with (-1). Disk probably does not exist.
] Successfully wrote to block 25 of disk disk3.dsk.
] Successfully wrote to block 39 of disk disk3.dsk.
] Successfully wrote to block 8 of disk disk3.dsk.
] Successfully wrote to block 9 of disk disk3.dsk.
] Successfully wrote to block 15 of disk disk3.dsk.
] Successfully wrote to block 21 of disk disk3.dsk.
] Successfully wrote to block 25 of disk disk3.dsk.
] Successfully wrote to block 33 of disk disk3.dsk.
] Successfully wrote to block 35 of disk disk3.dsk.
] Successfully wrote to block 42 of disk disk3.dsk.
```

## Step 3:
Test what you have written to disk0.dsk, disk1.dsk, disk2.dsk, disk3.dsk by running executable again:
```
./diskTest
```


Correct Output:
```
] Existing disk disk0.dsk opened.
] Previous writes were varified. Now, delete the .dsk files if you want to run this test again.
] Existing disk disk1.dsk opened.
] Previous writes were varified. Now, delete the .dsk files if you want to run this test again.
] Existing disk disk2.dsk opened.
] Previous writes were varified. Now, delete the .dsk files if you want to run this test again.
] Existing disk disk3.dsk opened.
] Previous writes were varified. Now, delete the .dsk files if you want to run this test again.
```

## Auxiliary Steps to verify content of disk files:


Testing Commands for specific block and offset in files:

```
hexdump -e'"%07.8_ax  " 8/1 "%02x " "  " 8/1 "%02x " "  |"' -e'16/1  "%_p"  "|\n"' disk0.dsk
```
- Hex dump everything and give decimal position as well as the bytes (in character form)


```
xxd -s 2304 -l 256 -p disk0.dsk
```
- Read from disk0.dsk 256 blocks offset by 2304 (i.e. logical number 9)


## Step 4:
Clean test
```
make cleanTest
```