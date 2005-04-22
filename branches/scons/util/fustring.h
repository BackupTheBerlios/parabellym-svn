/*
 * Faerion RunTime Library.
 * Copyright (c) 2003-2004 hex@faerion.oss and others.
 * Distributed under the terms of GNU LGPL, read 'LICENSE'.
 *
 * $Id$
 *
 */

#ifndef __faeutil_string_h
#define __faeutil_string_h

#include <stddef.h> /* for size_t */

namespace funs {

unsigned int atou(const char *);
void strlcpy_local(char *, const char *, size_t);
void strlcpya(char *, size_t, ...);

}

using namespace funs;

#endif /* __faeutil_string_h */
