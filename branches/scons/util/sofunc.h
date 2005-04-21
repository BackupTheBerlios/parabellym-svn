/*
 * Faerion RunTime Library.
 * Copyright (c) 2003-2004 hex@faerion.oss and others.
 * Distributed under the terms of GNU LGPL, read 'LICENSE'.
 *
 * $Id$
 *
 * This file contains macros that simplify working with
 * shared objects on different platforms.
 *
 */

#ifndef __parabellym_util_sofunc_h
#define __parabellym_util_sofunc_h

#ifdef _WIN32
# include <windows.h>
# define os_modfncmp stricmp
# define os_loadmod(name) (void*)LoadLibrary(name)
# define os_freemod(hand) FreeLibrary((HINSTANCE)hand)
# define os_getprocaddr(hand, name) GetProcAddress((HINSTANCE)hand, name)
#else
# include <dlfcn.h>
# define os_modfncmp strcmp
# define os_loadmod(name) (void*)dlopen(name,RTLD_NOW|RTLD_GLOBAL)
# define os_freemod(hand) dlclose(hand)
# define os_getprocaddr(hand, name) dlsym(hand, name)
# define os_moderror dlerror
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(os_moderror)
char * os_moderror(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __parabellym_util_sofunc_h */
