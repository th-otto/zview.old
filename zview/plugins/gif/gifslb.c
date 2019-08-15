/*
 * gifslb.c - initialization code for the shared library
 *
 * Copyright (C) 2019 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright file.
 */

#include <stdio.h>
#include <mint/basepage.h>
#include <mint/mintbind.h>
#include <mint/slb.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "zview.h"
#include "plugin.h"
#include "plugin_version.h"

#if defined(__MSHORT__) || defined(__PUREC__) || defined(__AHCC__)
# error "the slb must not be compiled with -mshort"
#endif

extern char const slb_header[];
static const BASEPAGE *my_base;

/*
 * referenced from header.S
 */
long slb_init(void);
void slb_exit(void);
long slb_open(BASEPAGE *bp);
void slb_close(BASEPAGE *bp);
long __CDECL slb_control(long fn, void *arg);



#define MINT_MAX_PID 999
#define MAX_PIDS (MINT_MAX_PID + 1)

struct per_proc {
	struct _zview_plugin_funcs *funcs;
	pid_t pid;
};

static struct per_proc procs[MAX_PIDS];

static pid_t slb_user(void)
{
	pid_t pid = Pgetpid();
	if (pid == (pid_t)-ENOSYS)
		pid = 1;
	return pid;
}


static struct per_proc *get_proc(pid_t pid, pid_t slot)
{
	int i;
	
	/*
	 * see if we can use it as a direct index into our array
	 */
	if (pid > 0 && pid < MAX_PIDS && procs[pid].pid == slot)
		return &procs[pid];
	/*
	 * TODO: if MiNT ever uses pids >= 1000,
	 * use a hash instead
	 */
	for (i = 1; i < MAX_PIDS; i++)
		if (procs[i].pid == slot)
			return &procs[i];
	return NULL;
}


__attribute__((__noinline__))
struct _zview_plugin_funcs *get_slb_funcs(void)
{
	pid_t pid = slb_user();
	struct per_proc *proc = get_proc(pid, pid);
	if (proc == NULL)
		return NULL;
	return proc->funcs;
}


/*
 * these are not optional and cannot be set
 * to zero in the header, even if they
 * currently don't do anything
 */
long slb_init(void)
{
	const BASEPAGE *bp;
	const long *exec_longs;

	bp = (BASEPAGE *)(slb_header - 256);
	exec_longs = (const long *)((const char *)bp + 28);
	if ((exec_longs[0] == 0x283a001aL && exec_longs[1] == 0x4efb48faL) ||
		(exec_longs[0] == 0x203a001aL && exec_longs[1] == 0x4efb08faL))
		bp = (const BASEPAGE *)((const char *)bp - 228);
	my_base = bp;
	return 0;
}


void slb_exit(void)
{
}


long slb_open(BASEPAGE *bp)
{
	pid_t pid = slb_user();
	struct per_proc *proc = get_proc(pid, 0);
	
	(void)(bp);
	if (proc == NULL)
		return -ENFILE; /* ENFILE: file table overflow, well, proc table in this case */
	/*
	 * check if SLB is already in use by this process;
	 * should not happen since MiNT should have taken care of that already
	 */
	if (proc->funcs != NULL)
		return -EACCES;
	
	proc->pid = pid;
	
	return 0;
}


void slb_close(BASEPAGE *bp)
{
	pid_t pid = slb_user();
	struct per_proc *proc = get_proc(pid, pid);

	(void)(bp);
	if (proc == NULL)
		return;
	proc->funcs = NULL;
	proc->pid = 0;
}


/*
 * get the function table pointer passed from the application.
 * Automatically done in slb_zlib_open()
 */
__attribute__((__noinline__))
static long set_imports(struct _zview_plugin_funcs *funcs)
{
	pid_t pid = slb_user();
	struct per_proc *proc = get_proc(pid, pid);

	if (proc == NULL)
		return -ESRCH;
	if (funcs->struct_size != sizeof(*funcs))
		return -EINVAL;
	if (funcs->plugin_version > PLUGIN_VERSION)
		return -EBADARG;
	if (funcs->int_size != sizeof(int))
		return -ERANGE;
	proc->funcs = funcs;

	return 0;
}


__attribute__((__noinline__))
static long slb_compile_flags(void)
{
	long flags = 0;
#if defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
    flags |= (1L << 16);
#endif
#if defined(__mcoldfire__)
    flags |= (1L << 17);
#endif
	return flags;
}


long __CDECL slb_control(long fn, void *arg)
{
	switch ((int)fn)
	{
	case 0:
		return slb_compile_flags();
	case 1:
		return set_imports(arg);
	case 2:
		return (long)my_base;
	case 3:
		return (long)slb_header;
	case 4:
		return (long)my_base->p_cmdlin;
	}
	return -ENOSYS;
}


/*
 * satisfy references from giflib,
 * which is currently linked statically
 */
#undef memcpy
void *memcpy(void *dest, const void *src, size_t len)
{
	return get_slb_funcs()->p_memcpy(dest, src, len);
}


#undef strlen
size_t strlen(const char *str)
{
	return get_slb_funcs()->p_strlen(str);
}


#undef strncmp
int strncmp(const char *s, const char *d, size_t len)
{
	return get_slb_funcs()->p_strncmp(s, d, len);
}


#undef memset
void *memset(void *dest, int c, size_t len)
{
	return get_slb_funcs()->p_memset(dest, c, len);
}


#undef memcmp
int memcmp(const void *d, const void *s, size_t len)
{
	return get_slb_funcs()->p_memcmp(d, s, len);
}


#undef malloc
void *malloc(size_t s)
{
	return get_slb_funcs()->p_malloc(s);
}


#undef calloc
void *calloc(size_t n, size_t s)
{
	return get_slb_funcs()->p_calloc(n, s);
}


#undef realloc
void *realloc(void *ptr, size_t size)
{
	return get_slb_funcs()->p_realloc(ptr, size);
}


#undef free
void free(void *p)
{
	get_slb_funcs()->p_free(p);
}


#undef close
int close(int fd)
{
	return get_slb_funcs()->p_close(fd);
}


#undef fread
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return get_slb_funcs()->p_fread(ptr, size, nmemb, stream);
}


#undef fclose
int fclose(FILE *fp)
{
	return get_slb_funcs()->p_fclose(fp);
}


#undef fdopen
FILE *fdopen(int fd, const char *mode)
{
	return get_slb_funcs()->p_fdopen(fd, mode);
}


#undef open
int open(const char *filename, int flags, ...)
{
	mode_t mode = 0644;
	va_list args;
	if (flags & O_CREAT)
	{
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	return get_slb_funcs()->p_open(filename, flags, mode);
}

#undef errno
int errno;
