#ifndef LIBA_BRIDGE_STRING_H
#define LIBA_BRIDGE_STRING_H

#include_next <string.h>

#include "../private/macros.h"

LIBA_BEGIN_DECLS

#ifndef __GLIBC__
size_t strlcpy(char * dst, const char * src, size_t len);
#endif

LIBA_END_DECLS

#endif
