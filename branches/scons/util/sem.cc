// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "ftspec.h"
#include "sem.h"

using namespace funs;

seml::seml()
{
	pcount = 0;
	pthread_cond_init(&cv, NULL);
}


seml::~seml()
{
	pthread_cond_destroy(&cv);
}


bool seml::waitex(mutex &mx, bool locked)
{
	bool rc;
	if (!locked)
		mx.enter();
	while (pcount == 0)
		rc = pthread_cond_wait(&cv, mx) ? true : false;
	--pcount;
	if (!locked)
		mx.leave();
	return rc;
}

bool seml::wait(mutex &mx, const ftspec &ts)
{
	::timespec pts;
	pts.tv_sec = ts.sec;
	pts.tv_nsec = ts.msec * 1000000;

#ifdef _WIN32
	pts.tv_nsec *= 1000;
#endif

	{
		mlock lock(mx);
		if (pcount == 0)
			errno = pthread_cond_timedwait(&cv, mx, &pts);
		else {
			errno = 0;
			pcount--;
		}
		return errno ? false : true;
	}
}


bool seml::post(mutex &mx, bool locked)
{
	bool rc;
	if (!locked)
		mx.enter();
	if (rc = pthread_cond_signal(&cv) ? false : true)
		pcount++;
	if (!locked)
		mx.leave();
	return rc;
}


bool seml::broadcast(mutex &mx, bool locked)
{
	bool rc;
	if (!locked)
		mx.enter();
	if (rc = pthread_cond_broadcast(&cv) ? false : true)
		pcount++;
	if (!locked)
		mx.leave();
	return rc;
}
