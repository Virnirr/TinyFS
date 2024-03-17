CC = gcc 
CFLAGS = -Wall -g -std=c99
PROG = tinyFSDemo
DTEST = diskTest

DOBJS = libDisk.o

OBJS = libTinyFS.o libDisk.o tinyFsDemo.o

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

tinyFsDemo.o: tinyFSDemo.c libTinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libTinyFS.o: libTinyFS.c libTinyFS.h libDisk.h libDisk.o TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libDisk.o: libDisk.c libDisk.h libTinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:	
	rm -f $(OBJS) *~ TAGS
	rm tinyFSDemo diskX.dsk second_disk tinyFSDisk

submission: libDisk.c libDisk.h libTinyFS.c libTinyFS.h Makefile README.md TinyFS_errno.h tinyFSDemo.c
	tar -cf project4_submission.tar libDisk.c libDisk.h libTinyFS.c libTinyFS.h Makefile README.md TinyFS_errno.h tinyFSDemo.c
	gzip project4_submission.tar