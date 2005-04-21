// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$

#ifndef __parabellym_mdisp_h
#define __parabellym_mdisp_h

#include "../util/list.h"
#include "../util/stock.h"
#include "api.h"
#include "msgq.h"

typedef void (__stdcall * para_modbody_fn)(int);

extern funs::tlskey tlsk;

// default function dispatcher class, provides limited access.
class mdisp
{
	friend class mdispb;
protected:
	// incoming message queue.
	msgq queue;
	// this packet stores the last received message.
	msgp lastmsg;
	// identifies the thread that is blocked by the last
	// received message; needs to be unblocked when the
	// current thread quits or calls msg_receive().
	mdisp *blocked;
	// this packet stores a response.
	msgp repd;
	// this is signalled when a response is available.
	funs::sem reps;
	// unblocks the thread that had sent us a message
	// that required a response.
	void unblock();
public:
	// initializers.
	mdisp();
	virtual ~mdisp();
	// supervisor calls.
	virtual int mod_load(const char *name);
	virtual int core_init();
	virtual int core_shutdown(enum para_unload_mode mode);
	// restricted calls
	virtual int msg_subscribe(const char *name);
	virtual int msg_unsubscribe(int msgid);
	virtual int msg_receive(para_msgi_t *info);
	virtual int msg_reply(const para_msgi_t *info);
	// generic calls
	virtual int msg_attach(int qid);
	virtual int msg_find(const char *name);
	virtual int msg_send(para_msgi_t *info);
	virtual int msg_signal(int qid, int signal);
	// checks whether the queue belongs to the module.
	virtual bool is_owner(const msgq::body *other) const { return (queue.is_valid() && queue == other); }
	virtual bool is_owner(class msgq &other) const { return (queue.is_valid() && queue == other); }
};

// module body dispatcher class.
class mdispb : public mdisp
{
	// module id (used for sending signals).
	int id;
	// real module body.
	para_modbody_fn func;
	// module handle, used to unload the library.
	void *handle;
	// a list of loaded modules.
	typedef funs::stock<mdispb *> stock_t;
	static stock_t stock;
	// stock semaphore, signalled when a module is removed.
	static funs::sem mrsem;
protected:
	// thread proc.
	void body();
public:
	// initializers.
	mdispb(para_modbody_fn func, void *handle);
	~mdispb();
	// thread proc wrapper.
	static void* bodyw(void *arg);
	// waits for all modules to unload.
	static void wait_unload();
	// broadcasts a signal (unrestricted).
	static void msg_signal_bc(int sig);
};

// supervisor dispatcher class (the first thread)
class mdisps : public mdisp
{
public:
	// initializers.
	mdisps();
	~mdisps();
	// supervisor calls.
	int mod_load(const char *name);
	int core_init();
	int core_shutdown(enum para_unload_mode mode);
	// sends a signal to a module (unrestricted).
	int msg_signal(int qid, int sig);
};

#endif // __parabellym_mdisp_h
