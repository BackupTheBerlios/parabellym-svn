// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// Message Queue implementation.  Read msgq.h for details.

#include <string.h>
#include <faeutil/stock.h>
#include <faeutil/list.h>
#include "api.h"
#include "debug.h"
#include "msgq.h"
#include "message.h"

using namespace faeutil;

// The main list of message queues.  Not exposed to outside.
static stock<msgq::body *> qstock;

msgq::msgq()
{
	q = NULL;
}

msgq::msgq(int qid)
{
	if (qstock.get(qid, q))
		q->attach();
	else
		q = NULL;
}

msgq::~msgq()
{
	if (q != NULL)
		q->detach();
}

msgq& msgq::operator = (const msgq &src)
{
	detach();

	if ((q = src.q) != NULL)
		q->attach();

	return *this;
}

msgq& msgq::operator = (int qid)
{
	detach();

	if (qstock.get(qid, q))
		q->attach();
	else
		q = NULL;

	return *this;
}

int msgq::get_id() const
{
	return (q != NULL) ? q->get_id() : 0;
}

void msgq::create()
{
	if (q != NULL)
		q->detach();
	q = new body();
}

void msgq::detach()
{
	if (q != NULL)
		q->detach();
	q = NULL;
}

int msgq::peek(msgp &mp, int &sig)
{
	if (q != NULL)
		return q->peek(mp, sig);
	return PEC_MSGQ_ERROR;
}

int msgq::post(const msgp &mp)
{
	if (q != NULL && q->post(mp))
		return PEC_SUCCESS;
	return PEC_MSGQ_ERROR;
}

int msgq::signal(int sig)
{
	if (q != NULL) {
		q->signal(sig);
		return PEC_SUCCESS;
	}
	return PEC_MSGQ_ERROR;
}

int msgq::signal_bc(int sig, const msgq &sender)
{
	unsigned int count = 0;

	for (stock<msgq::body *>::iterator it = qstock.begin(); it != qstock.end(); ++it) {
		if (it->id > 0 && it->body != sender.q) {
			it->body->signal(sig);
			++count;
		}
	}

	return count ? PEC_SUCCESS : PEC_MSG_NO_RECIPIENT;
}

int msgq::subscribe(const char *name)
{
	if (!is_valid())
		return PEC_MSGQ_BAD_ID;
	return q->subscribe(name);
}

int msgq::unsubscribe(int msgid)
{
	if (!is_valid())
		return PEC_MSGQ_BAD_ID;
	return q->unsubscribe(msgid);
}

msgq::body::body()
{
	id = qstock.add(this);
}

msgq::body::~body()
{
	qstock.remove(id);
}

void msgq::body::attach()
{
	mlock lock(mx);
	++refc;
}

bool msgq::body::detach()
{
	unsigned int tmp;

	mx.enter();
	tmp = --refc;
	mx.leave();

	if (refc == 0) {
		delete this;
		return true;
	}

	return false;
}

int msgq::body::peek(msgp &mp, int &sig)
{
	log((flog, flMmq, "%x: attempting to retreive a packet", this));

	{
		mlock lock(mx);
		log((flog, flMmq, "%x: locked up, popping out", this));

		if (siglist.begin() == siglist.end() && packetlist.begin() == packetlist.end()) {
			log((flog, flMmq, "%x: no data, waiting up", this));
			psem.waitex(mx, true);
		}

		if (siglist.begin() != siglist.end()) {
			if (siglist.pop_front(sig)) {
				log((flog, flMmq, "%x: signal %d dequeued", this, sig));
				return PEC_SIGNAL;
			} else {
				log((flog, flMmq, "%x: could not dequeue a signal -- unexpected", this));
				return PEC_MSGQ_ERROR;
			}
		}

		if (packetlist.pop_front(mp)) {
			log((flog, flMmq, "%x: dequeued a packet, all ok", this));
			return PEC_SUCCESS;
		}
	}

	log((flog, flMmq, "%x: peeked nothing", this));
	return PEC_MSGQ_ERROR;
}

bool msgq::body::post(const msgp &mp)
{
	log((flog, flMmq, "%x: posting a packet", this));

	{
		mlock lock(mx);
		log((flog, flMmq, "%x: locked up, pushing in", this));

		packetlist.push_back(mp);
		psem.post();
	}

	log((flog, flMmq, "%x: posted ok", this));
	return true;
}

void msgq::body::signal(int sig)
{
	mlock lock(mx);
	siglist.push_back(sig);
	psem.post();
}

int msgq::body::subscribe(const char *name)
{
	int rc;

	if (name == NULL || strlen(name) == 0)
		return PEC_INVALID_ARGUMENT;

	if ((rc = message::subscribe(name, this)) >= 0) {
		roster.add(rc);
		log((flog, flMod, "%x: subscribed ok.", this));
	} else {
		log((flog, flMod, "%x: could not subscribe -- error %x", this, rc));
	}
	
	return rc;
}

int msgq::body::unsubscribe(int msgid)
{
	roster.remove(msgid);
	return message::unsubscribe(msgid, this);
}
