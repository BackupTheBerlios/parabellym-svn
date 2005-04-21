// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// Message Queue Packet implementation.  Read msgq.h for details.

#ifdef _DEBUG
# include <assert.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "../util/boif.h"
#include "../util/sem.h"
#include "api.h"
#include "debug.h"
#include "msgq.h"


static void putraw(void *&dst, const void *src, size_t size)
{
	memcpy(dst, src, size);
	dst = reinterpret_cast<char *>(dst) + size;
}


static void getraw(void *&src, void *dst, size_t size)
{
	if (dst != NULL)
		memcpy(dst, src, size);
	src = reinterpret_cast<char *>(src) + size;
}


msgp::msgp()
{
	data = NULL;
}


msgp::msgp(const para_msgi_t *info, class mdisp* blocked)
{
	void *tmp;
	size_t size =
		sizeof(size_t) +
		sizeof(blocked) +
		sizeof(para_msgi_t);

	for (unsigned int idx = 0; idx < info->attc; ++idx)
		size += info->attv[idx].size + sizeof(para_iov_t);

	if ((tmp = data = malloc(size)) == NULL) {
		size = 0;
		return;
	}

	faeutil_putints(&tmp, size);
	faeutil_putptrs(&tmp, blocked);
	putraw(tmp, info, sizeof(para_msgi_t));
	putraw(tmp, info->attv, sizeof(para_iov_t) * info->attc);

	for (unsigned int idx = 0; idx < info->attc; ++idx)
		putraw(tmp, info->attv[idx].data, info->attv[idx].size);

#ifdef _DEBUG
	{
		size_t diff = reinterpret_cast<char *>(tmp) - reinterpret_cast<char *>(data);
		if (diff != size)
			log((flog, flMsg, "packet @%p MISMATCH: wrote %u bytes instead of %u as expected.\n", data, static_cast<unsigned int>(diff), static_cast<unsigned int>(size)));
		assert(diff == size);
	}
#endif
}


void msgp::map(para_msgi_t *info, class mdisp *& blocked)
{
	void *tmp = data;

	faeutil_getints(&tmp, NULL); // ignore the size
	faeutil_getptrs(&tmp, reinterpret_cast<void **>(&blocked));

	// retreive the information block
	getraw(tmp, info, sizeof(para_msgi_t));

	// the array of vectors comes next, point it
	info->attv = reinterpret_cast<para_iov_t *>(tmp);

	// skip the array itself.
	getraw(tmp, NULL, sizeof(para_iov_t) * info->attc);

	// remap the attachments.
	for (unsigned int idx = 0; idx < info->attc; ++idx) {
		info->attv[idx].data = tmp;
		getraw(tmp, NULL, info->attv[idx].size);
	}
}


void msgp::clone(const msgp &src)
{
	if (src.is_empty()) {
		data = NULL;
	} else {
		unsigned int sz;
		faeutil_getint(src, &sz);
		data = malloc(sz);
		memcpy(data, src, sz);
	}
}


void msgp::free()
{
	if (data != NULL) {
		::free(data);
		data = NULL;
	}
}
