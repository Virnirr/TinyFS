#define ENOENT -1 // no such file or directory

#define EEXIST -2 // file exist
#define EFAULT -3 // bad address

#define EBADF -4 // bad file descriptor

#define EROFS -5 // read-only filesystem
#define EWOFS -6 // write-only filesystem


/* MISC. error codes */
#define EMAX_INT -7 // over INT_MAX limit