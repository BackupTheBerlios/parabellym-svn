// Faerion RunTime Library.
// Copyright (c) 2003-2004 hex@faerion.oss and others.
// Distributed under the terms of GNU LGPL, read 'LICENSE'.
//
// $Id$

#include "mutex.h"

using namespace funs;

tlskey::tlskey()
{
	pthread_key_create(&_host, NULL);
}


tlskey::~tlskey()
{
	pthread_key_delete(_host);
}


bool tlskey::set(const void *arg)
{
	return pthread_setspecific(_host, arg) == 0;
}


void * tlskey::get()
{
	return pthread_getspecific(_host);
}
