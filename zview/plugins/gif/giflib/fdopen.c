#include <stdio.h>

#if defined(__PUREC__) && !defined(_MINTLIB_SOURCE)
FILE *fdopen(int sfd, const char *mode)
{
	FILE *newfd;

	newfd = fopen("CON:", mode);
	if (newfd)
	{
		newfd->Handle = sfd;
		newfd->Flags |= 0x08|0x80; /* _FIOBUF|_FIOBIN */
	}
	return newfd;
}
#endif
