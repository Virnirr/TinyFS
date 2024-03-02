


int openDisk(char *filename, int nBytes) {
  /* This functions opens a regular UNIX file and designates the first
    nBytes of it as space for the emulated disk. If nBytes is not exactly a
    multiple of BLOCKSIZE then the disk size will be the closest multiple
    of BLOCKSIZE that is lower than nByte (but greater than 0) If nBytes is
    less than BLOCKSIZE failure should be returned. If nBytes > BLOCKSIZE
    and there is already a file by the given filename, that file’s content
    may be overwritten. If nBytes is 0, an existing disk is opened, and the
    content must not be overwritten in this function. There is no requirement
    to maintain integrity of any file content beyond nBytes. The return value
    is negative on failure or a disk number on success. */
    int disk_fd;

    if (nBytes < BLOCKSIZE && nBytes > 0) {
      perror("nBytes is less than BLOCKSIZE failure");
      return -1;
    }
    // If nBytes is not multiple of BLOCKSIZE, set it as multiple of BLOCKSIZE lower than nBytes, but > 0
    if (nBytes > BLOCKSIZE || !(nBytes % BLOCKSIZE)) {
      nBytes = nBytes - (nBytes % BLOCKSIZE);
      total_disk_space = total_disk_space;

      // Prepare a block of zeros
      char zeros[nBytes] = {0}; // This initializes all elements to 0

      // open or create filename
      disk_fd = open(filename, OFLAGS, RWPERMS);
      if (disk_fd < 0) {
        perror("open disk filename");
        return -1;
      }

      // Write the nBytes of zeros to the file
      size_t written = write(disk_fd, sizeof(zeros), nBytes);
      if (written < sizeof(zeros)) {
          perror("Failed to write zeros to file");
          return -1;
      }

    }

    // If nBytes is 0, an existing disk is opened
    if (nBytes == 0) {
      // not sure what happens.
    }

    return diskNumber++;
}

int closeDisk(int disk) {
  /*
  Takes a file descriptor disk and close it
  
  @param int disk: file descriptor of an open file
  @return int: returns 0 on Success, -1 on failure
  */

  // close tar file 
  if (close(disk) < 0) {
    perror("close");
    return -1;
  }
  return 0;
}

int readBlock(int disk, int bNum, void *block) {
  /*  reads an entire block of BLOCKSIZE bytes from the open
  disk (identified by ‘disk’) and copies the result into a local buffer
  (must be at least of BLOCKSIZE bytes)
  
  @params int disk: file descriptor relating to a disk
  @params int bNum: a logical block number (bNum = 0 is very first byte of the file)
  @params void *block: 

   On success, it returns 0. 
   -1 or smaller is returned if disk is not available (hasn’t been opened) or any other failures. 
   You must define your own error code system. 
  */

  // seeks to the logical number position on disk
  int byte_offset = bNum * BLOCKSIZE;
  if (lseek(disk, SEEK_SET, byte_offset) < 0) {
    perror("lseek readBlock");
    return -1;
  }

  // reads from disk of the logical position and copies the result into buffer block
  num_read = read(disk, block, BLOCKSIZE);
  if (num_read < 0) {
    perror("read");
    return -1;
  }

  return 0;
}

void writeBlock(int disk, int bNum, void *block) {
  /*
  Takes diks number (fd) and logical number bNum and writes the content of the buffer "block" to that location

  Returns 0 on success, or -1 on failure
  */
    // seeks to the logical number position on disk
  int byte_offset = bNum * BLOCKSIZE;
  if (lseek(disk, SEEK_SET, byte_offset) < 0) {
    perror("lseek readBlock");
    return -1;
  }

  // reads from disk of the logical position and copies the result into buffer block
  num_read = write(disk, block, BLOCKSIZE);
  if (num_read < 0) {
    perror("read");
    return -1;
  }

  return 0;
}