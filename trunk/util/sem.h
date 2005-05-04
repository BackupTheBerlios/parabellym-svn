// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss and others.
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$
//
// This class is a wrapper around semaphores, as defined by the
// POSIX 1003.1b-1993 (POSIX.1b) standard.  The main purpose of
// the class is to automate initialization and destruction of the
// corresponding system objects.

#ifndef __funs_sem_h
#define __funs_sem_h

#include "mutex.h"

namespace funs
{

class ftspec;

// The `sem' class is a stand-alone full-featured semaphore,
// while its light version -- `seml' -- doesn't have an own
// mutex and needs one external.

class seml
{
	pthread_cond_t cv;
	// The number of signals not yet dequeued.
	unsigned int scount;
	// The number of waiting threads.  Used by broadcast()
	// to signal this much times.
	unsigned int wcount;
public:
	seml();
	~seml();
	bool waitex(mutex &mx, bool locked = false);
	bool wait(mutex &mx) { return waitex(mx, false); }
	bool wait(mutex &mx, const ftspec &ts);
	bool broadcast(mutex  &mx);
	bool post(mutex &mx);
};


class sem : private seml
{
public:
	mutex mx;
public:
	// Initialization.
	sem() { }
	~sem() { }
	bool wait() { return seml::wait(mx); }
	bool wait(const ftspec &ts) { return seml::wait(mx, ts); }
	bool waitex(bool locked = false) { return seml::waitex(mx, locked); }
	bool broadcast() { return seml::broadcast(mx); }
	bool post() { return seml::post(mx); }
};

};

#endif // __funs_sem_h
