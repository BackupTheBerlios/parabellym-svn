// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// This file provides a simple function that loads the core dynamically,
// without having to link to it.  The purpose of this file is to make
// it available as a single function to be used in different kind of
// loaders without dealing with the OS-dependent code for loading shared
// objects.
//
// $Id$

#include "sofunc.h"
#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
# include <unistd.h>
#endif
#include "../core/api.h"

#if defined(_WIN32)
# define init_func_name "para_core_init@0"
# define sleep(x) Sleep(x * 1000)
#else
# define init_func_name "para_core_init"
#endif

typedef int (* para_core_init_fn)(const char *);

// Here we store the handle to the module
static void *hInst = NULL;

int para_core_load(const char *appname)
{
	int rc = PEC_INIT_FAILURE;

	hInst = os_loadmod("libparabellym." MODEXT);

	if (hInst == NULL)
		hInst = os_loadmod("paracore." MODEXT);

	if (hInst != NULL) {
		para_core_init_fn fn = (para_core_init_fn)os_getprocaddr(hInst, init_func_name);

		if (fn != NULL)
			rc = fn(appname);
		else {
			fprintf(stderr, "para_load_core: %s\n", os_moderror());
		}

		if (rc != PEC_SUCCESS) {
			os_freemod(hInst);
			hInst = NULL;
		}
	} else {
		fprintf(stderr, "para_load_core: %s\n", os_moderror());
	}

	return rc;
}


int para_core_unload(enum para_unload_mode mode)
{
	int rc = para_core_shutdown(mode);

	if (rc != PEC_SUCCESS)
		return rc;

	return PEC_SUCCESS;
}
