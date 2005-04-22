// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// This file contains all exported functions.

#include "debug.h"
#include "mdisp.h"
#include "message.h"

static const struct errmsg {
	int id;
	const char *msg;
} errtab[] = {
	{ PEC_MSGQ_BAD_ID, "bad queue id" },
	{ PEC_MSGQ_EMPTY, "queue is empty" },
	{ PEC_MSGQ_ERROR, "general queue failure" },
	{ PEC_MSG_NOT_FOUND, "message not found" },
	{ PEC_MSG_BAD_ID, "bad message id" },
	{ PEC_MSG_FAILED, "general message failure" },
	{ PEC_MOD_FAILED, "general module failure" },
	{ PER_MOD_INVALID, "invalid module" },
	{ PEC_MOD_NOT_LOADED, "module not found" },
	{ PEC_MOD_ENTRY_MISSING, "the module does not have a para_mod_body function" },
	{ PEC_MOD_REQUIRED, "called from outside a module's main thread" },
	{ PEC_MOD_BAD_ID, "bad module id" },
	{ PEC_SIGNAL, "interrupted by a signal" },
	{ PEC_INVALID_ARGUMENT, "invalid argument" },
	{ PEC_MEMORY_ERROR, "memory error" },
	{ PEC_INIT_FAILURE, "initialization failure" },
	{ PEC_HAVE_MODULES, "some modules are active" },
	{ PEC_NOT_IMPLEMENTED, "not implimented" },
	{ PEC_UNAUTHORIZED, "the current thread is not authorized for this operation" },

	{ PEC_SUCCESS, "success" }
};

funs::tlskey tlsk;

static mdisp* mod_restore(void)
{
	static mdisp *super = NULL;
	mdisp *md = reinterpret_cast<mdisp *>(tlsk.get());

	if (md == NULL) {
		if (super == NULL)
			super = md = new mdisps();
		else
			md = new mdisp();
		tlsk.set(md);
	}

	return md;
}

PARA_API(const char *) para_errstr(int id)
{
	const char *msg = NULL;

	for (unsigned int idx = 0; idx < sizeof(errtab) / sizeof(errtab[0]); ++idx) {
		if (errtab[idx].id == id) {
			msg = errtab[idx].msg;
			break;
		}
	}

	if (msg == NULL)
		msg = "unknown error";

	return msg;
}

PARA_API(int) para_core_init(void)
{
	return mod_restore()->core_init();
}

PARA_API(int) para_core_shutdown(enum para_unload_mode mode)
{
	return mod_restore()->core_shutdown(mode);
}

PARA_API(int) para_mod_load(const char *modname)
{
	return mod_restore()->mod_load(modname);
}

PARA_API(int) para_msg_attach(int qid)
{
	return mod_restore()->msg_attach(qid);
}

PARA_API(int) para_msg_find(const char *msgname)
{
	return mod_restore()->msg_find(msgname);
}

PARA_API(int) para_msg_receive(para_msgi_t *info)
{
	return mod_restore()->msg_receive(info);
}

PARA_API(int) para_msg_reply(const para_msgi_t *info)
{
	return mod_restore()->msg_reply(info);
}

PARA_API(int) para_msg_send(para_msgi_t *info)
{
	return mod_restore()->msg_send(info);
}

PARA_API(int) para_msg_signal(int modid, int sig)
{
	return mod_restore()->msg_signal(modid, sig);
}

PARA_API(int) para_msg_subscribe(const char *msgname)
{
	return mod_restore()->msg_subscribe(msgname);
}

PARA_API(int) para_msg_unsubscribe(int msgid)
{
	return mod_restore()->msg_unsubscribe(msgid);
}
