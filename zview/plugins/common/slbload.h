#include <mint/slb.h>

/* OS independend SLB loader prototypes */
long slb_load(const char *sharedlib, const char *path, long min_ver, SLB_HANDLE *slb, SLB_EXEC *slbexec);
long slb_unload(SLB_HANDLE slb);
void slb_setpath(const char *userpath);

#ifndef __mcoldfire__
void asm_invalidate_cache(void);
#endif
