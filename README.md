*This is a copy of README in newlib/libc/sys/hixs/README. Note that no code is in master-branch.*

                HIXS
				====

Introduction
------------
HIXS stands for "Hooked Interface to *niXs Systems" and is an extension 
for the newlib library. It is intended to aid the system developper in the 
effort of developing customized targets adaptions.

It is meant to be used as more flexible alternative to libgloss and/or
in cases where a target architecture that is supported in general for 
newlib, but where newlib lacks a "system" for it (either high level system 
in libc or 

HIXS main benefit is the ability to replace the system-calls without 
re-compiling the whole tool-chain (or Newlib for that matter). Additionally,
it also provides a powerfull monitoring functionality that makes it very
easy to see what system-calls are invoked by the higher layers (i.e. Newlib)
and to trigger watch/break-points for further stepping inside Newlib.

HIXS is a spin-off to the Tinker kernel project and most Tinker BSP 
templates are based on HIXS (http://www.sf.net/tinker). However, HIXS 
is meant to be usable without any dependency to any kernel and should work 
quite happily alongside with any kernel, or with no kernel at all if required
(i.e. single-threaded bare-boned embedded applications).

Why HIXS
--------
Normally when you use GNU tools you'll end up having to provide the 
tool-chain with a number of function calls or rely on some "system" 
abstraction allready written by someone else.

These function calls are called system calls and are needed as a gateway 
between resources of the target and the tools (compiler, libraries e.t.a.).

Even though this works very well it has the following major drawbacks:

1) 	Inflexible - any final tool-chain will have a strong relation 
	to the target it's built for. Historically this was not thought
	to be an issue because there were only a limited number of systems
	available. With todays rapid development on the embedded market,
	there are as many system permutations as there are projects.
2)	When you need one or several system calls differently implemented
	you'll end up having to alter the newlib source, which most likely
	will lead to the need of maintaining your particular "system"
	outside of the main newlib source repository.
3)	Alternatively you can overload the functions of a existing 
	pre-built tool-chain at the linking stage. But in that case you have 
	to be carefull about the order the object files when you finally link 
	then or you might end up using the original call without knowing it. 
	Also, you'll have remember to provide --allow-multiple-definition,
	which in turn might omit other important errors in parts of your
	code not related to the systemcalls.

How it works
------------
Adapting a tool-chain to HIXS is very easy and is almost certain to work 
with any CPU architecture that is supported by newlib. If you get past the
first stage of gcc and if the CPU is supported by newlib in general, you
can almost count on that HIXS will work for you.

One of the main innovations builds on the idea of interpreting the canonical 
name of a target slightly differently than usual. Normally the canonical name 
is interpreted as follows:

<CPUarchitecture-Vendor-OS> - (for example "x86-gnu-linux" or "powerpc-elf")

Often the vendor part is often omitted. Furthermore, the vendor part is in most 
ignored by the build process by the GNU-tools (especially pure embedded 
tool-chains not targeting fully featured OS) and this is what HIXS 
exploits.

HIXS interpret the cannoncal name as follows instead:

<CPUarchitecture-system-ABI> - (for example "powerpc-hixs-elf")
Where ABI stands for Application Binary Interface and among others, defines the 
calling convention.

(In embedded cases this is usually what you'd want from the "OS" part anyway -
hence the names "elf", "eabi" e.t.a.).

To summarize: You enable HIXS for your tool-chain by by adding the name 
*-hixs-* in the middle of your canonical name for your target you intent to 
build a tool-suite for. 

For example, to adapt a powerpc/elf tool-chain you pass --target=powerpc-hixs-elf
to the configure script for binutils, gcc, newlib and gdb. Then just build and 
install as usual. *-hixs-* will be ignored by all the tools build except newlib, 
where the system-calls layer will be glued in.

How to use HIXS
----------------
For each system call newlib or any other built-in library expects, there is a 
corresponding HIXS function pointer called hixs_<syscallname>. For example: 
for the function "open" there is a corresponding function pointer called 
hixs_open. Since this is a variable, you assign this variable the value of your 
own "open". The syscalls are listed below.

All these function pointers are also preset to stubbed but "functional" calls 
that HIXS also provide. (The primarily for this is to enable the build of the 
tool-chain to complete from binutils to gdb.) To reroute for example "write" to 
your own version My_write, all you have to do is (preferably somewhere in your 
start-up code):

hixs_write = My_write; 

To aid you in the process of developping a fully functional complete adaption,
HIXS also provide yet another function called hixs_syscal_mon. This function 
takes a function pointer as it's input argument. 

If you reroute this one too, you can set a break-point in it and inspect the
argument. This is very practical and highly recommended. You will see exactly 
how and in which order higher abstraction functions operate (for example 
printf).

The final code for our simple example could look something like this:

	int My_write (int file, char *ptr, int len)
	{
		len=console_write(ptr, len);	// Your low level primitive driver
		return len; 
	};
	
	void (*used_syscall)(void);
	void My_syscall_mon(void *hix_syscall)	{
		used_syscall = hix_syscall;
	} //<-- Set break-point here, inspect value of "used_syscall"
	
	
	int main(int argc, char** argv)
	{
		hixs_write = My_write; 
		hixs_syscall_mon  = My_syscall_mon;
		printf("Hello world\n");
	}



Assuming you have a function "console_write" that actually outputs something, 
you now suddenly have a fully functional printf for your target!

Special considerations:
-----------------------
1)
When you use HIXS you pay a slight performance penalty because each system 
call will result in 4 extra calls. First the call to the HIXS syscall function,
this is only a wrapper which calls the hixs_syscall function. But before that
it also calls the hixs_syscall_Mon, which in turn follows the same principle as 
the other system calls (i.e. it's overloadable and calls your version of the 
syscall_Mon).

In practice this is often negligible, because real-fife a system will not that
much time calling system calls in comparison with other duties. When you're 
happy with your calls and you wish to optimize performance, all you have to do
is to rename them to the corresponding real names and rebuild your tool-chain 
without HIXS but using them instead.

2)
Function pointers are by nature placed in voilatable memory (RAM). They are
thereby also vulnerable for program bugs that corrupt memory. In normal case
in an embedded application, a function is placed in ROM and "is not affected" 
by this kind of bugs.

It this kind of bug occurs with HIXS, your system will crash badly. Normally
this is not an issue anyway, because a system error of that kind will almost 
certainly manifest itself badly somewhere else instead.

3)
With HIXS nothing stops you from re-routing system calls in a running system.
Don't do that! - It's not a good practice unless you're really an expert.
Suppose you do that in a multithreaded application or while an interrupt is
using the function, returning from that function will be unpredictable at best.
When you use HIXS, always reroute the calls as part of your initialization and 
then leave it untouched.


System calls
------------

Newlib required			HIXS provides		Note
================		==============		====
close				hixs_close
				hixs_exit		1.
execve				hixs_execve
fcntl				hixs_fcntl		2. 5.
fork				hixs_fork
fstat				hixs_fstat
getpid				hixs_getpid
				hixs_isatty		4.
gettod				hixs_gettimeofday	3.
kill				hixs_kill
link				hixs_link
lseek				hixs_lseek
open				hixs_open
read				hixs_read
sbrk				hixs_sbrk
				hixs_settimeofday	1.
stat				hixs_stat
times				hixs_times
unlink				hixs_unlink
wait				hixs_wait
write				hixs_write

Foot-notes for the table above:
1)	Newlib does not depend on this function, but either gcc does, or it's
	provided for your convenience.
2)	Newlib provides a stubbed and non-functional version of this function.
	For HIXS to be able to over-load fcntl, HAVE_FCNTL must be defined 
	when newlib is built (this is done internally when you build newlib 
	for HIXS - i.e. you don't do this yourself).
3)	The list created by the command above will produce the name
	'gettod', but the function is really called 'gettimeofday'.
4)	This function is mistakenly not in the syscalls directory. Finding 
	out that newlib depends upon it can only be done by trial-and-error.
5)	ANSI states last argument(s) are optional but newlib headers has a 
	fixed number of arguments. Always call this function according to
	newlib definition (i.e. with all the arguments mentioned/required).

Tips:
The system calls that newlib depends upon can be listed by invoking:

ls -al newlib/libc/syscalls/ | grep sys | sed -e 's/^.*sys//' | \
sed -e 's/.c$//' | sort

This will give a correct list for all but one or two API's that are either 
named differently or left out. (Which ones are pointed out in the table above.)

Dynamic memory allocation:
--------------------------
For dynamic memory allocation you'll need either to overload the sbrk function
(i.e. hook on your own), or you can keep the HIXS version.

In either case you have to provide the symbol 'end' or your link will fail.
But in the first case  this can be just any dummy symbol.

In the second case, you'll keep the HIXS provided sbrk function, but special
considerations must be taken about the 'end' symbol.

The end symbol should be provided by the linker and you *must* make sure 
that it comes last in the memory region you place it in or you will risk
overwriting other valid data when using your allocated memory blocks.
The region you place the 'end' symbol in must also be R/W.

Note that printf family of functions normally use dynamic memory.

Normally your tool-chain will produce a lot of various section-names and it 
can be hard (and risky) to mention them all explicitly, so here comes an 
example of a method to guarantee that the requirement is met.

	.text :
	{
		_code_start = .;
		*(.exception);
		*(.text);
	} >ROM

	.data :
	{
		*(.data) *(COMMON);
	} >RAM

	.sbss :
	{
		__SBSS_BEGIN__ = .;
		*(.sbss)	/* static variables */
		__SBSS_END__ = .;
	} >RAM

	.bss :
	{
		*(.bss)
	} >RAM

	.myheap :
	{
		end = .;
	} >RAM

Note that the section .myheap does not really exist. The tools will not
generate any section with this name (if they do, choose another name).

By placing this last among any of the outbut sections that will relocate
to the same region 'RAM', you'll be guaranteed that any unmentioned 
sections that are places by default in either .data, .bss or .sbss
comes before this symbol.

IMPORTANT:
Also note that the section .sbss must be initialized by your startup
code. sbrk will detect a cold boot by detecting that it's internal
heap-pointer has a value of zero. If it's not, it will provide any alloc 
function with an random address which is definitely not what you'd want.

HIXS with other embedded OS's
------------------------------
Enabling a tool-chain can also be made for tool-chains allready adapted 
for existing kernels like for example RTEMS. The canonical name for such
a target could look something like this: powerpc-hixs-rtems.

Having a tool-chain built for both RTEMS and HIXS is a little bit of a 
contradiction in terms since both aim to provide you with a "system".

However, if you do this - HIXS will take presidency over RTEMS for the 
system specific parts and you'll end up with a HIXS "system", Why would you 
want that? In some cases it can be beneficial to build for RTEMS since 
you might benefit from improvements that RTEMS provides you with that is 
not related to any low-level system issues. This could for example be 
multi-lib support or specific CPU related things. Or it could be that
your application relies on RTEMS (or any other embedded kernel), but you
need a system-call monitor to aid your debugging for something below the 
kernel level.

Configure options and other tweaks
-----------------------------------
Since the newlib build-system will identify HIXS early in the configure, 
adaptions related to a specific CPU might be omitted. The build/configure
structure when using HIXS is very forgiving and might in some cases miss
important settings related to your architecture or OS when these in normal
cases are meant to be deductable by newlib itself. If this is happens, 
first of all try to find out if you can enable/disable that setting explicitly 
by using  configure flags. If that doesn't work, try adding the options to 
CFLAGS prior your configure of newlib. If all that fail - you'll have to modify 
./newlib/configure.host (and possibly, but not likely ./configure.in) with a 
more specific filter. Please send me a patch an that case...

crt0.o
======
The really observant user will notice that HIXS puts it's glue layer in the
GNU startup module crt0.o. Why so, since it just as easily could be part of
libc.a?

The main benefit of doing this is that when your target adaption is finished
you won't be needing HIXS any longer. But instead of building a new tool-chain
without HIXS, you can as an intermediate step just compile your own crt0.o and
let the linker use that instead. If you wrap your system-calls using a
nifty wrapper, all you need to do is changing that macro, remove and/or replace
crt0.o and you have a finalized system that has the system calls hard-linked.

