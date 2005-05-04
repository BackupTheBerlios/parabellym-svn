// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "sem.h"
#include "ftspec.h"

using namespace funs;

seml::seml()
{
	pthread_cond_init(&cv, NULL);
}


seml::~seml()
{
	pthread_cond_destroy(&cv);
}


bool seml::waitex(mutex &mx, bool locked)
{
	bool rc = false;
	if (!locked)
		mx.enter();
	wcount++;
	while (scount == 0)
		rc = pthread_cond_wait(&cv, mx) ? true : false;
	wcount--;
	scount--;
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
		bool rc = false;
		mlock lock(mx);
		wcount++;
		while (scount == 0)
			rc = pthread_cond_timedwait(&cv, mx, &pts) ? false : true;
		wcount--;
		scount--;
		return rc;
	}
}


bool seml::post(mutex &mx)
{
	mlock lock(mx);
	scount += 1;
	return pthread_cond_signal(&cv) ? false : true;
}


bool seml::broadcast(mutex &mx)
{
	mlock lock(mx);
	scount += wcount;
	return pthread_cond_broadcast(&cv) ? false : true;
}
