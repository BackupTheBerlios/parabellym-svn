// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include "boif.h"

namespace funs {

void faeutil_putint(void *to, unsigned int value)
{
	unsigned char *tmp = reinterpret_cast<unsigned char *>(to);

	*tmp++ = (value >> 24) & 0xFF;
	*tmp++ = (value >> 16) & 0xFF;
	*tmp++ = (value >>  8) & 0xFF;
	*tmp++ = (value      ) & 0xFF;
}


void faeutil_putints(void **to, unsigned int value)
{
	faeutil_putint(*to, value);
	*to = reinterpret_cast<void *>(reinterpret_cast<char *>(*to) + 4);
}


unsigned int faeutil_getint(void *from, unsigned int *to)
{
	unsigned int value;
	unsigned char *tmp = reinterpret_cast<unsigned char *>(from);

	value  = (*tmp++ & 0xFF) << 24;
	value |= (*tmp++ & 0xFF) << 16;
	value |= (*tmp++ & 0xFF) <<  8;
	value |= (*tmp   & 0xFF)      ;

	if (to != NULL)
		*to = value;

	return value;
}


unsigned int faeutil_getints(void **from, unsigned int *to)
{
	unsigned int rc = faeutil_getint(*from, to);
	*from = reinterpret_cast<void *>(reinterpret_cast<char *>(*from) + 4);
	return rc;
}

}
