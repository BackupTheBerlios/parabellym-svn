// Copyright (c) 2003-2004 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// The supervisor thread dispatcher.

#include <string.h>
#include "../util/sofunc.h"
#include "debug.h"
#include "mdisp.h"
#include "message.h"

#ifndef dimof
# define dimof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

mdisps::mdisps()
{
}

mdisps::~mdisps()
{
}

int mdisps::core_init()
{
#if defined(_DEBUG) && defined(HAVE_flog_writef)
	debug_init();
#endif
	return PEC_SUCCESS;
}

int mdisps::core_shutdown(enum para_unload_mode mode)
{
	log((flog, flGen, "%x: shutting down using mode %d", this, mode));

	switch (mode) {
	case PUC_WAIT:
		mdispb::wait_unload();
		log((flog, flGen, "all modules have quit, ja mata"));
		return PEC_SUCCESS;
	case PUC_EMERGENCY:
		msgq::signal_bc(PSIG_UNLOAD, queue);
		return PEC_SUCCESS;
	default:
		return PEC_HAVE_MODULES;
	}
}

int mdisps::mod_load(const char *name)
{
	void *handle = os_loadmod(name);
	static const char *funcs[] = { "para_mod_body@4", "para_mod_body" };

	if (handle == NULL) {
		log((flog, flMod, "could not load module %s: %s", name, os_moderror()));
		return PEC_MOD_NOT_LOADED;
	}

	for (unsigned int idx = 0; idx < dimof(funcs); ++idx) {
		para_modbody_fn func = (para_modbody_fn)(os_getprocaddr(handle, funcs[idx]));

		if (func != NULL) {
			pthread_t tmp;
			mdispb *mod = new mdispb(func, handle);

			if (pthread_create(&tmp, NULL, mdispb::bodyw, mod) != 0) {
				log((flog, flMod, "could not spawn worker thread for module %s", name));
				delete mod;
				return PEC_MOD_FAILED;
			}

			log((flog, flMod, "loaded module %x (%s)", mod, name));
			return PEC_SUCCESS;
		}
	}

	log((flog, flMod, "module %s not loaded: no entry point", name));
	return PEC_MOD_ENTRY_MISSING;
}

int mdisps::msg_signal(int qid, int sig)
{
	msgq tmp = qid;
	if (!tmp.is_valid() || tmp == queue)
		return PEC_MSGQ_BAD_ID;
	return tmp.signal(sig);
}
