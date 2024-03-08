#define MOUNT_SUCCESS 11 // no such file or directory
#define UNMOUNT_SUCCESS 10 // no such file or directory
#define ENOENT -1 // no such file or directory

#define EEXIST -2 // file exist
#define EFAULT -3 // bad address

#define EBADF -4 // bad file descriptor

#define TFS_EFO -5;

#define EROFS -5 // read-only filesystem
#define EWOFS -6 // write-only filesystem


/* MISC. error codes */
#define EMAX_INT -7 // over INT_MAX limit

#define EMINLIMIT -8

#define NOT_A_FILE_SYSTEM -10
#define UMOUNT_FAIL -11
#define READBLOCK_FAIL -12
#define OFFSET_FAIL -13 //seek offset isn't valid
#define CLOSEDISK_FAIL -14 //seek offset isn't valid

#define LIMIT_REACHED -15