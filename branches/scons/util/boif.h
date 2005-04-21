/*
 * Faerion RunTime Library.
 * Copyright (c) 2003-2004 hex@faerion.oss and others.
 * Distributed under the terms of GNU LGPL, read 'LICENSE'.
 *
 * $Id$
 *
 */

#ifndef __parabellym_util_boif_h
#define __parabellym_util_boif_h

#include <stddef.h> /* for size_t */

void faeutil_putshort(void *, unsigned short);
unsigned short faeutil_getshort(void *, unsigned short *);
void faeutil_putint(void *, unsigned int);
unsigned int faeutil_getint(void *, unsigned int *);
void faeutil_putptr(void *, void *);
void * faeutil_getptr(void *, void **);

void faeutil_putshorts(void **, unsigned short);
unsigned short faeutil_getshorts(void **, unsigned short *);
void faeutil_putints(void **, unsigned int);
unsigned int faeutil_getints(void **, unsigned int *);
void faeutil_putptrs(void **, void *);
void * faeutil_getptrs(void **, void **);

#endif /* __parabellym_util_boif_h */
