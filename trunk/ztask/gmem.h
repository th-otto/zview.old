#ifndef GMEM_H
#define GMEM_H

#ifdef __cplusplus
extern "C" {
#endif

extern void *gmalloc(int size);
extern void *grealloc(void *p, int size);
extern void gfree(void *p);
extern void gMemReport( void);
char *copyString(char *s);
#ifdef __cplusplus
}
#endif


#endif

