// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include "boif.h"

void faeutil_putshort(void *to, unsigned short value)
{
	unsigned char *tmp = reinterpret_cast<unsigned char *>(to);

	*tmp++ = (value >>  8) & 0xFF;
	*tmp++ = (value      ) & 0xFF;
}


void faeutil_putshorts(void **to, unsigned short value)
{
	faeutil_putshort(*to, value);
	*to = reinterpret_cast<void *>(reinterpret_cast<char *>(*to) + 2);
}


unsigned short faeutil_getshort(void *from, unsigned short *to)
{
	unsigned short value;
	unsigned char *tmp = reinterpret_cast<unsigned char *>(from);

	value  = (*tmp++ & 0xFF) <<  8;
	value |= (*tmp   & 0xFF)      ;

	if (to != NULL)
		*to = value;

	return value;
}


unsigned short faeutil_getshorts(void **from, unsigned short *to)
{
	unsigned short rc = faeutil_getshort(*from, to);
	*from = reinterpret_cast<void *>(reinterpret_cast<char *>(*from) + 2);
	return rc;
}
