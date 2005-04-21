// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// MESSAGE QUEUE.

#ifndef __parabellum_core_msgq_h
#define __parabellum_core_msgq_h

#include <faeutil/list.h>
#include <faeutil/sem.h>
#include "msgp.h"

class msgq
{
	// Only allow modules to create and destroy queuees.
	friend class mdispb;
public:
	class body;
private:
	// The queue itself.
	body *q;
public:
	// Initialization.  Only resets the id.  To actually
	// create one, the create() method must be called.
	msgq();
	// Initialization.  Attaches to a specified queue.
	msgq(int qid);
	// Destruction.  Decrements reference counter for
	// the attached queue and, if zero, destroys it.
	~msgq();
	// Returns queue id.
	int get_id() const;
	// Copies the queue identifier.  Increments the
	// reference counter, if attached to a  valid queue.
	msgq& operator = (const msgq &src);
	// Attaches to a queue.
	msgq& operator = (int qid);
	// Posts a message.
	int post(const msgp &);
	// Retreives a message.
	int peek(msgp &, int &sig);
	// Inserts a packet into the beginning of the queue.
	int reply(const msgp &);
	// Post a signal.
	int signal(int sig);
	static int signal_bc(int sig, const msgq &sender);
	// Compares queues.
	inline bool operator == (const msgq::body *src) const { return (q == src); }
	inline bool operator == (const msgq &src) const { return (q == src.q); }
	// Checks whether this is a valid queue.
	inline bool is_valid() const { return (q != NULL); }
	// Subscribes the queue for a message.
	int subscribe(const char *name);
	// Unsubscribes the queue from a message.
	int unsubscribe(int msgid);
protected:
	// Creates a new queue.
	void create();
	// Detaches from the current queue.
	void detach();
};

// Internal 
class msgq::body
{
	friend class msgq;
	// Reference counter.
	unsigned int refc;
	// Queue id.
	int id;
	// the list of messages this queue receives.
	faeutil::list <int> roster;
protected:
	// Packet list mutex.  Locked when peeking or poking
	// data to the queue.
	faeutil::mutex mx;
	// This semaphore is signalled when a new message
	// is available in the queue.
	faeutil::seml psem;
	// The list of packets.
	faeutil::list_ut<msgp> packetlist;
	// The list of signals.  Quite like packets, but
	// with a higher priority and with no data payload.
	faeutil::list_ut<int> siglist;
	// Incremente reference counter.
	void attach();
	// Decrements reference counter.  Returns true if refc becomes zero.
	bool detach();
public:
	body();
	~body();
	// Returns queue id.
	int get_id() const { return id; }
	// Inserts a message into the queue and waits for it to
	// be processed, if the message requires an answer.
	// Otherwise the message is copied to the queue and the
	// function returns immediately.  Call with psem.mx being
	// locked.
	bool post(const msgp &mp);
	// Retreives a message.  If the message is not
	// immediately available, sleeps.  Call with psem.mx being
	// locked.
	int peek(msgp &mp, int &sig);
	// Simulates a signal.
	void signal(int sig);
	// Subscription management.
	int subscribe(const char *);
	int unsubscribe(int);
};

#endif // __parabellum_core_msgq_h
