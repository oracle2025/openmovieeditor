#include <string.h>

#include "strlcpy.h"

int strlcpy(char *dst, const char *src, int dstsize)
{
	if (strlen(src) < dstsize) {
		strcpy(dst,src);
	} else {
		strncpy(dst,src,dstsize-1);
		dst[dstsize-1] = '\0';
	}
	return strlen(src);
}

