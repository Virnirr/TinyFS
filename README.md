Author: Zhixiang He, Sean Phun, Dennis Phun

# TinyFS and Disk Overview

![](/static/tinyfs-overview.png)

# Disk Emulator Implementation

A “block device emulator” library that allows for block operations like opening, reading, and writing to a disk. The hard disk drives implementation uses regular Unix file as storage.

# TinyFS Design Decisions and Implementations

### 1. Superblock

Contains magic_number (i.e. 0x44), address of root, and the next free block chained in a linked list (explained later)

### 2. Inode Block

Contains: block_type, magic_number, file_type (i.e. directory or regular file), filename (max: 8 char), file size, creation access and modification time, pointer to the first file extent, and the read only bit. For our regular file implementation, we decided to utilize the "linked list" approach where all connecting file extent are linked one after another using our "TinyFS Global Index" to index into a certain data block. Each connecting link is connected with a number offset as said earlier and the last one would point to -1, indicating "EOF". Our inode stores the creation time, access time, and modification time in order to implement one of the additional features. The same goes for read only bit.

### 3. File Extent Block

Our file extent block contains the block type, magic number, the next file extent (i.e. global linked list index), and the file content. If the file content of a specific file goes beyond a single block, then a new block is created and the current block will point to the newly created block randomly in our TinyFS

### 4. Free Block

Our free block contains block type, magic number, the next free block (in linked list format) and a series of NULL. Similar to file extent, all free blocks are chained together with a linked list of global TinyFS file offset to easily find the next free block. The first free block is stored in the superblock so it is easily accessible when needed.

# An explanation of how well your TinyFS implementation works, including tradeoffs you made and why

Overall, our TinyFS implementation worked great with extensive testing in our demo. Some of the tradeoffs we had to make was choosing linked list implementation to store contiguous blocks of memory. This is a good implementation at the beginning where contents are sparse and contains no fragmentation. However, as the file system gets larger and larger, linked list implementation might slow down as it needs time to probe through the chained of linked blocks.

# Additional Funtionality:

The additional functions we decided on are:
1. Directory listing and file renaming

Directory listing works by going through the whole TinyFS and listing any regular file's name. File renaming goes through the whole TinyFS and changes the name of the file.

2. Read-only and writeByte support

Initially, all the writes (writeFile, writeByte, writeByte_offset) work since the files are read-write by default. After changing it to read-only, all of them fail. And changing it back makes them work once again.

3. Timestamps

For timestamps, we created a file and displayed its creation time, then slept for 1 second and created another file after it and displayed its creation time.


4. Implement file system consistency checks:

A newly created filesystem is able to be mounted. However, after we corrupt the superblock by overwriting it with whatever junk values are initialized by a buffer, the mount fails. This is a rather simple test for our complete implementation, but we weren't sure how else to test it. We also made sure that each block had the correct structure and values (i.e. block type, null in rest, etc.), each next free block was actually a free block, each file extent was a file extent, and the total blocks added up to the correct value. 

Similar to the MVP interface of TinyFS, additional features have a suit of testing in our tinyFSDemo.c that shows how it works and why it works.

# Limitations and bugs

As far as we have tested most of the features and functionalities, we have not found any bugs thus far.
