/*
 *  HIXS Fake crt0
 *
 *  Each BSP provides its own crt0 and linker script.  Unfortunately
 *  this means that crt0 and the linker script are not available as
 *  each tool is configured.  Without a crt0 and linker script, some
 *  targets do not successfully link "conftest.c" during the configuration 
 *  process.  So this fake crt0.c provides all the symbols required to
 *  successfully link a program.  The resulting program will not run
 *  but this is enough to satisfy the autoconf macro AC_PROG_CC. 
 */

void HIXS_provides_crt0( void ) {}  /* dummy symbol so file always has one */

/*We want the HIXS funtion pointers to be part of crto.o so that user can 
omitt/replade syscall with his own versions later. Therefor we include the 
source here
it*/
#include "hixsys.c"


//Hmm, whats the proper name for this? FIXME
//#if defined(__ARM__)
void  initialise_monitor_handles (void){}
//#endif



#if defined(__rtems__)
/*
This is for cases where you have a HIXS enabled tool-chain for the
for the RTEMS kernel. I.e. powerpc-hixs-rtems, blackfin-hixs-rtems e.t.a.

NOTE: Please note that most of these symbols must be redefined in the 
linker script for your tool-chain to produce functional code (i.e. these 
definitions are enough for successfully linking "conftest.c" during the 
configuration process of the second gcc build phase, but will not produce 
runnable code).
*/

/* The PowerPC expects certain symbols to be defined in the linker script. */
#if defined(__PPC__)
  int __SDATA_START__;  int __SDATA2_START__;
  int __GOT_START__;    int __GOT_END__;
  int __GOT2_START__;   int __GOT2_END__;
  int __SBSS_END__;     int __SBSS2_END__;
  int __FIXUP_START__;  int __FIXUP_END__;
  int __EXCEPT_START__; int __EXCEPT_END__;
  int __init;           int __fini;
  int __CTOR_LIST__;    int __CTOR_END__;
  int __DTOR_LIST__;    int __DTOR_END__;
  int _SDA_BASE_;       int _SDA2_BASE_;
#endif

/* The SH expects certain symbols to be defined in the linker script. */

#if defined(__sh__)
int __EH_FRAME_BEGIN__;
#endif

/*  The hppa expects this to be defined in the real crt0.s. 
 *  Also for some reason, the hppa1.1 does not find atexit()
 *  during the AC_PROG_CC tests.
 */

#if defined(__hppa__)
/*
  asm ( ".subspa \$GLOBAL\$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=40");
  asm ( ".export \$global\$" );
  asm ( "\$global\$:");
*/

  asm (".text");
  asm (".global");
  asm (".EXPORT $$dyncall,ENTRY");
  asm ("$$dyncall:");
  int atexit(void (*function)(void)) { return 0; }
#endif


/*
 *  The AMD a29k generates code expecting the following.
 */

#if defined(_AM29000) || defined(_AM29K)
asm (".global V_SPILL, V_FILL" );
asm (".global V_EPI_OS, V_BSD_OS" );

asm (".equ    V_SPILL, 64" );
asm (".equ    V_FILL, 65" );

asm (".equ    V_BSD_OS, 66" );
asm (".equ    V_EPI_OS, 69" );
#endif

#if defined(__AVR__)
/*
 * Initial stack pointer address "__stack"
 *  hard coded into GCC instead of providing it through ldscripts
 */
const char* __stack ;
#endif

#endif /* defined(__rtems__) */



