CC = gcc 
CFLAGS = -Wall -g -std=c99
PROG = tinyFSDemo
DTEST = diskTest

DOBJS = libDisk.o

OBJS = tinyFSDemo.o libTinyFS.o libDisk.o 

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

test: dTest clean

cleanTest:
	rm -f *.dsk diskTest

dTest: $(DOBJS)
	$(CC) $(CFLAGS) -o $(DTEST) $(DOBJS)

diskTest.o: diskTest.c libDisk.h
	$(CC) $(CFLAGS) -c -o $@ $<

tinyFsDemo.o: tinyFSDemo.c tinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libTinyFS.o: libTinyFS.c tinyFS.h libDisk.h libDisk.o TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

libDisk.o: libDisk.c libDisk.h tinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:	
	rm -f $(OBJS) *~ TAGS