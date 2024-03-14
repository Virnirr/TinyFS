CC = gcc 
CFLAGS = -Wall -g -std=c99
PROG = tinyFSDemo
DTEST = diskTest

DOBJS = libDisk.o

OBJS = libTinyFS.o libDisk.o tfsTest.o

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

test: dTest clean

cleanTest:
	rm -f *.dsk diskTest

dTest: $(DOBJS)
	$(CC) $(CFLAGS) -o $(DTEST) $(DOBJS)

tfsTest.o: tfsTest.c libTinyFS.h
	$(CC) $(CFLAGS) -c -o $@ $<

diskTest.o: diskTest.c libDisk.h
	$(CC) $(CFLAGS) -c -o $@ $<

tinyFsDemo.o: tinyFSDemo.c libTinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libTinyFS.o: libTinyFS.c libTinyFS.h libDisk.h libDisk.o TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libDisk.o: libDisk.c libDisk.h libTinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:	
	rm -f $(OBJS) *~ TAGS
	rm disk1 disk2