// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#ifdef _DEBUG
# include <stdio.h>
#endif
#include <string.h>
#include "../util/crc32.h"
#include "../util/fustring.h"
#include "../util/stock.h"
#include "api.h"
#include "debug.h"
#include "message.h"
#include "mdisp.h"

using namespace funs;

// This structure is passed to all functions that want a message name.
// Ensures that (1) exclusive time is not wasted on hint calculation
// and (2) the message is a valid C string in a readable memory region.
struct msgid
{
	int code;
	const char *name;
	unsigned int hint;
	msgid(const char *name)
	{
		this->code = PEC_MSG_BAD_ID;
		this->name = name;
		this->hint = crc32(name, strlen(name));
	}
	bool operator == (const msgid &src)
	{
		return (code == src.code);
	}
};

// Cached memory allocators.
IMPLEMENT_ALLOCATORS(message);

// The list of registered messages.
static stock<message *> msglist;
typedef stock<message *>::iterator i_msglist;

message::message(msgid &id)
{
	strlcpy(this->name, id.name, sizeof(this->name));
	name_hint = id.hint;
	code = id.code = msglist.add(this);
}

message::~message()
{
	msglist.remove(code);
}

bool message::find(msgid &id, message **msg)
{
	bool rc = false;

	for (i_msglist it = msglist.begin(); it != msglist.end(); ++it) {
		if (it->id >= 0 && it->body->name_hint == id.hint && strcmp(it->body->name, id.name) == 0) {
			if (msg != NULL)
				*msg = it->body;
			id.code = it->id;
			rc = true;
			break;
		}
	}

	return rc;
}

int message::find(const char *name)
{
	msgid id(name);
	mlock lock(msglist.mx);
	return find(id, NULL) ? id.code : PEC_MSG_NOT_FOUND;
}

int message::subscribe(const char *name, msgq::body *q)
{
	message *msg;
	msgid id(name);
	mlock lock(msglist.mx);

	if (!find(id, &msg)) {
		msg = new message(id);
		if (id.code < 0) {
			delete msg;
			msg = NULL;
		}
	}

	if (msg == NULL)
		return PEC_MSGQ_ERROR;

	if (!msg->roster.add(q))
		return PEC_MSGQ_BAD_ID;

	return id.code;
}


int message::unsubscribe(int code, msgq::body *q)
{
	message *msg;
	mlock lock(msglist.mx);

	if (!msglist.get(code, msg))
		return PEC_MSG_BAD_ID;

	if (!msg->roster.remove(q))
		return PEC_MSGQ_BAD_ID;

	if (msg->roster.size() == 0)
		delete msg;

	return PEC_SUCCESS;
}


int message::send(para_msgi_t *info, mdisp *sender)
{
	message *msg;
	list_ut<msgq::body *> queues;
	unsigned int count = 0;

	log((flog, flMsg, "%x: fetching the list of subscribers", info->msgid));
	msglist.mx.enter();
	if (msglist.get(info->msgid, msg))
		msg->roster.copy(queues);
	msglist.mx.leave();
	log((flog, flMsg, "%x: done, found %d subscribers", info->msgid, queues.size()));

	if (queues.size() == 0) {
		log((flog, flMsg, "%x: noone to send to", info->msgid));
		return PEC_MSG_BAD_ID;
	} else for (list_ut<msgq::body *>::iterator it = queues.begin(); it != queues.end(); ++it) {
		if (sender->is_owner(*it)) {
			log((flog, flMsg, "%x: refusing to send to the sender -- skipped", info->msgid));
			continue;
		}
		++count;
		log((flog, flMsg, "%x: posting to a module", info->msgid));
		(*it)->post(msgp(info, info->rreq ? sender : NULL));
		if (info->rreq)
			break;
	}

	log((flog, flMsg, "%x: sent", info->msgid));
	return (info->rreq && count == 0) ? PEC_MSG_NO_RECIPIENT : PEC_SUCCESS;
}
