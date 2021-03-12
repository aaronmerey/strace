/* This file is used to test the 'catch syscall' feature on GDB.

   Please, if you are going to edit this file DO NOT change the syscalls
   being called (nor the order of them).  If you really must do this, then
   take a look at catch-syscall.exp and modify there too.

   Written by Sergio Durigan Junior <sergiodj@linux.vnet.ibm.com>
   September, 2008

   This is the gdb catch-syscall.c test */

#include "tests.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sched.h>

#if defined(__arm__)
/* Although 123456789 is an illegal syscall umber on arm linux, kernel
   sends SIGILL rather than returns -ENOSYS.  However, arm linux kernel
   returns -ENOSYS if syscall number is within 0xf0001..0xf07ff, so we
   can use 0xf07ff for unknown_syscall in test.  */
int unknown_syscall = 0x0f07ff;
#else
int unknown_syscall = 123456789;
#endif

/* Set by the test when it wants execve.  */
int do_execve = 0;


int
main (int argc, char *const argv[])
{
	int fd[2];
	char buf1[2] = "a";
	char buf2[2];
	/* Satisfy static checks of the form:
	   ignoring return value declared with attribute warn_unused_result */
	int rc;

	/* Test a simple self-exec, but only on request.  */
	if (do_execve)
		rc += execv (*argv, argv);

	/* A close() with a wrong argument.  We are only
	   interested in the syscall.  */
	rc += close (-1);

	rc += chroot (".");

	rc += pipe (fd);

	rc += write (fd[1], buf1, sizeof (buf1));

	rc += read (fd[0], buf2, sizeof (buf2));

	/* Test fork-event interactions.  Child exits immediately. */
	if (fork () == 0)
	  _exit (0);
	else
	{
		/* Trigger an intentional ENOSYS.  */
		syscall (unknown_syscall);
		/* The last syscall.  Do not change this.  */
		_exit (0);
	}
	return rc;
}
