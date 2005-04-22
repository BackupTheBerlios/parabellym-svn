// Faerion RunTime Library.
// Copyright (c) 2002-2004 hex@faerion.oss and others.
// $Id: strlcpy.cc 32 2005-04-19 00:29:36Z vhex $

#include "fustring.h"

namespace funs {

void strlcpy_local(char *dst, const char *src, size_t size)
{
	while (size != 0 && *src != '\0') {
		*dst++ = *src++;
		--size;
	}

	if (size == 0)
		--dst;

	*dst = '\0';
}

}
