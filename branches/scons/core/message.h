// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// The `message' class contains the identification data of a registered
// message and the list of subscribers.  It does not have its own
// message queue.  Insted, it appends all sent messages to queues of all
// subscribers.

#ifndef __parabellym_message_h
#define __parabellym_message_h

#include <faeutil/mutex.h>
#include <faeutil/memory.h>
#include <faeutil/list.h>
#include "msgq.h"

class message
{
public:
	typedef unsigned int hint_t;
private:
	// Memory manager.
	DECLARE_ALLOCATOR;
	// Finds a message.
	static bool find(struct msgid &, message **);
	// Message code, used in destructor.
	int code;
protected:
	// Memory management.
	DECLARE_ALLOCATORS;
	// Message name.
	char name[64];
	// Message name hint (crc32).
	hint_t name_hint;
	// The list of subscribed queues.
	faeutil::list<msgq::body *> roster;
	// Initialization.
	message(struct msgid &);
	~message(void);
public:
	static int subscribe(const char *msgname, msgq::body *);
	static int unsubscribe(int code, msgq::body *);
	static int find(const char *msgname);
	static int send(para_msgi_t *info, class mdisp *sender = NULL);
};

#endif // __parabellym_message_h
