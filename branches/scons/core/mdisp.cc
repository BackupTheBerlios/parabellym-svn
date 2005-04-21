// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#include "debug.h"
#include "mdisp.h"
#include "message.h"

mdisp::mdisp()
{
}

mdisp::~mdisp()
{
	unblock();
	lastmsg.free();
}

void mdisp::unblock()
{
	if (blocked == NULL)
		return;
	log((flog, flMod, "%x: sending an empty response to a blocked module %x", this, blocked));
	msg_reply(NULL);
	blocked = NULL;
}

int mdisp::core_init()
{
	return PEC_UNAUTHORIZED;
}

int mdisp::core_shutdown(enum para_unload_mode)
{
	return PEC_UNAUTHORIZED;
}

int mdisp::mod_load(const char *)
{
	return PEC_UNAUTHORIZED;
}

int mdisp::msg_attach(int qid)
{
	msgq tmp = qid;

	if (!tmp.is_valid())
		return PEC_MSGQ_BAD_ID;

	queue = tmp;
	return PEC_SUCCESS;
}

int mdisp::msg_find(const char *msg)
{
	return message::find(msg);
}

int mdisp::msg_receive(para_msgi_t *info)
{
	int rc, sigid;
	lastmsg.free();

	unblock();

	if ((rc = queue.peek(lastmsg, sigid)) == PEC_SUCCESS) {
		lastmsg.map(info, blocked);
	}

	if (rc == PEC_SIGNAL) {
		info->signal = sigid;
	}

	return rc;
}

int mdisp::msg_reply(const para_msgi_t *info)
{
	para_msgi_t empty;

	if (blocked == NULL)
		return PEC_MSG_BAD_ID;

	if (info == NULL) {
		empty.attc = 0;
		empty.attv = NULL;
		info = &empty;
	}

	blocked->repd = msgp(info, NULL);
	blocked->reps.mx.enter();
	blocked->reps.post();
	blocked->reps.mx.leave();
	blocked = NULL;

	return PEC_SUCCESS;
}

int mdisp::msg_send(para_msgi_t *info)
{
	int rc;

	if (info->rreq)
		repd.free();

	log((flog, flMod, "%x: sending a message%s", this, info->rreq ? " (requires a response)" : ""));

	reps.mx.enter();

	if ((rc = message::send(info, this)) == PEC_SUCCESS && info->rreq) {
		int msgid;
		mdisp *unused;

		log((flog, flMod, "%x: waiting for a response", this));

		// Restore the message id (which is overwritten by
		// msgp::map), because the application doesn't expect
		// it to change.
		msgid = info->msgid;
		reps.waitex(true);
		repd.map(info, unused);
		info->msgid = msgid;

		// erase the data that may not have arrived with a response.
		info->rreq = false;
		info->signal = 0;

		log((flog, flMod, "%x: response arrived", this));
	}

	reps.mx.leave();

	log((flog, flMod, "%x: message %s", this, rc == PEC_SUCCESS ? "sent" : "failed"));
	return rc;
}

int mdisp::msg_signal(int qid, int sig)
{
	if (sig != PSIG_UNLOAD) {
		msgq q = qid;
		if (!q.is_valid())
			return PEC_MSGQ_BAD_ID;
		return q.signal(sig);
	}

	return PEC_UNAUTHORIZED;
}

int mdisp::msg_subscribe(const char *name)
{
	return queue.subscribe(name);
}

int mdisp::msg_unsubscribe(int msgid)
{
	return queue.unsubscribe(msgid);
}
