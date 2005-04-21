// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#include "debug.h"

#if defined(_DEBUG) && defined(HAVE_flog_writef)
flog_t flog = 0;

static const flogdef_t flogdefs[] = {
	{ "core.gen", "parabellym.log", 1, 1, 1 },
	{ "core.mod", "parabellym.log", 1, 1, 1 },
	{ "core.msg", "parabellym.log", 1, 1, 1 },
	{ "core.mmq", "parabellym.log", 1, 1, 1 },
};

void debug_init(void)
{
	flog = flog_init(flogdefs, sizeof(flogdefs) / sizeof(flogdefs[0]));
	log((flog, flGen, "initialized."));
}
#endif
