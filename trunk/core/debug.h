// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#ifndef __para_core_debug_h
#define __para_core_debug_h

#if defined(_DEBUG) && defined(HAVE_flog_writef)
# define log(str) flog_writef str
# include <flog.h>
extern flog_t flog;
enum flEnum
{
	flGen,
	flMod,
	flMsg,
	flMmq,
};
#else
# define log(str)
#endif

void debug_init(void);

#endif // __para_core_debug_h
