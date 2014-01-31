#include "hixs.h"
#include <errno.h> 
#include <sys/stat.h>

#define RX_BUFFLEN	102 
#define TX_BUFFLEN	102

//Syscall monitoring stuff
#if defined( _NO_HIXS_SYSCALLMON_ ) 
   #define _syscall_mon(x) ((void)0)
#else
   //The following declaration must follow the same signature as 
   //HIXS_syscall_mon and the monitoring variable.
   void _syscall_mon( void *syscall );

   //Defines function type for the monitoring variable
   DEF_FUN_HIXS(void, syscall, (void *)); 

   //The monitoring variable itself
   static pHIXS_syscall _hixs_lastcall;

   void HIXS_syscall_mon( void *syscall ) {
      _hixs_lastcall = syscall;
   }
#endif


//The real stubs follows from here:

int HIXS_close(int file) {
   return -1;
}

void HIXS_exit (int status) {
   while(1); 
};

/**
@name environ
Points to a list of environment variables and their values. For a minimal
environment, the following empty list is adequate. 

@note Not used yet. Kept for future reference. FIXME
*/
//@{
/*
char *__env[1] = { 0 };
char **environ = __env;
*/
//@}

int HIXS_execve(char *name, char **argv, char **env) {	
   errno=ENOMEM;
   return -1;
}

/* Third argument is optional, but is usually 'int arg' */
int HIXS_fcntl (int filedes, int command, ...){
   errno = ENOSYS;
   return -1;
}

int HIXS_fork() {
   errno=EAGAIN;
   return -1;
}

int HIXS_fstat(int file, struct stat *st) {
   st->st_mode = S_IFCHR;
   st->st_blksize = TX_BUFFLEN;
   return 0;
}

int HIXS_getpid() {
   return 1;
}

int HIXS_gettimeofday (struct timeval *tp, struct timezone *tzp) {
   errno=ENOSYS;
   return -1;
}

int HIXS_isatty(int file) {
   return 1;
}

int HIXS_kill(int pid, int sig) {
   errno=EINVAL;
   return(-1);
}

int HIXS_link(char *old, char *new) {
   errno=EMLINK;
   return -1;
}

int HIXS_lseek(int file, int ptr, int dir) {
   return 0;
}

/* Third argument is optional (mode_t mode) */
int HIXS_open(const char *filename, int flags, ...){
   errno = ENOSYS;
   return -1;
}

int HIXS_read(int file, char *ptr, int len) {
   return 0;
}

//Having a fairly working _sbrk really makes life much easier
caddr_t HIXS_sbrk(int incr) {
   extern char end;
   /* Defined by the linker. */
   static char *heap_end;
   char *prev_heap_end;

   if (heap_end == 0) {
      heap_end = &end;
   }
   prev_heap_end = heap_end; 
/*
   if (heap_end + incr > stack_ptr)
   {
      _write (1, "Heap and stack collision\n", 25);
      abort ();
   }
*/
   heap_end += incr;   
   return (caddr_t) prev_heap_end;   
}

int HIXS_settimeofday (const struct timeval *tp, const struct timezone *tzp) {
   errno=ENOSYS;
   return -1;
}

int HIXS_stat(const char *file, struct stat *st) {
   st->st_mode = S_IFCHR;
   st->st_blksize = TX_BUFFLEN;
   return 0;
}

clock_t HIXS_times(struct tms *buf) {
   return -1;
}

int HIXS_unlink(char *name) {
   errno=ENOENT;
   return -1;
}

int HIXS_wait(int *status) {
   errno=ECHILD;
   return -1;
}

int HIXS_write(int file, char *ptr, int len) {
    int todo;

    _syscall_mon(HIXS_write);
    for (todo = 0; todo < len; todo++) {
       //writechar(*ptr++);
    }
    return len;
}

