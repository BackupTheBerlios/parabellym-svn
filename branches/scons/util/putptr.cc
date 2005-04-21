// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include "boif.h"

void faeutil_putptr(void *to, void *value)
{
	long _value = reinterpret_cast<long>(value);
	unsigned char *tmp = reinterpret_cast<unsigned char *>(to);

#if 0
	if (sizeof(void *) == 8) {
		*tmp++ = (_value >> 56) & 0xFF;
		*tmp++ = (_value >> 48) & 0xFF;
		*tmp++ = (_value >> 40) & 0xFF;
		*tmp++ = (_value >> 32) & 0xFF;
	}
#endif

	*tmp++ = (_value >> 24) & 0xFF;
	*tmp++ = (_value >> 16) & 0xFF;
	*tmp++ = (_value >>  8) & 0xFF;
	*tmp++ = (_value      ) & 0xFF;
}


void faeutil_putptrs(void **to, void *value)
{
	faeutil_putptr(*to, value);
	*to = reinterpret_cast<void *>(reinterpret_cast<char *>(*to) + 4);
}


void * faeutil_getptr(void *from, void **to)
{
	long value = 0;
	unsigned char *tmp = reinterpret_cast<unsigned char *>(from);

#if 0
	if (sizeof(void *) == 8) {
		value |= (*tmp++ & 0xFF) << 56;
		value |= (*tmp++ & 0xFF) << 48;
		value |= (*tmp++ & 0xFF) << 40;
		value |= (*tmp++ & 0xFF) << 32;
	}
#endif

	value |= (*tmp++ & 0xFF) << 24;
	value |= (*tmp++ & 0xFF) << 16;
	value |= (*tmp++ & 0xFF) <<  8;
	value |= (*tmp   & 0xFF)      ;

	if (to != NULL)
		*to = reinterpret_cast<void *>(value);

	return reinterpret_cast<void *>(value);
}


void * faeutil_getptrs(void **from, void **to)
{
	void *rc = faeutil_getptr(*from, to);
	*from = reinterpret_cast<void *>(reinterpret_cast<char *>(*from) + 4);
	return rc;
}
