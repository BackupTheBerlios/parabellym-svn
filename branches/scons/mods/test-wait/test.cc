// parafile: simple profile storage for Parabellym
// Copyright (C) 2003 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// This is the first part of the parabellym test suite.  This module
// listens for an incoming message and exits when it arrives.

#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#else
# include <unistd.h>
#endif
#include "../../core/api.h"

static const char *msg = "oss.faerion.parabellym.test";

static bool phase1(const int msgid)
{
	int rc;
	bool quit = false;
	para_msgi_t mi;

	printf(" W < waiting for incoming messages.\n");

	if ((rc = para_msg_receive(&mi)) < 0) {
		if (rc == PEC_SIGNAL) {
			printf(" W < received signal %d, exiting.\n", mi.signal);
			return true;
		}

		printf(" W < could not receive message, error %d.  dead.\n", rc);
		return false;
	}

	if (mi.msgid != msgid) {
		printf(" W < unknown message received, ignoring\n");
		return false;
	}

	if (mi.attc > 0) {
		printf(" W < message: attc=%u, rreq=%s, signal=%d, text=\"%s\".\n", mi.attc, mi.rreq ? "true" : "false", mi.signal, mi.attv[0].as_str());
		if (strcmp(reinterpret_cast<const char *>(mi.attv[0].data), "end") == 0)
			quit = true;
	} else {
		printf(" W < message: attc=0, rreq=%s, signal=%d.\n", mi.rreq ? "true" : "false", mi.signal);
	}

	if (mi.rreq) {
		para_msgi_t ri;
		char text[] = "a response";
		para_iov_t arr(text, strlen(text) + 1);

		ri.attc = 1;
		ri.attv = &arr;

		printf(" W < response required, sending \"a response\".\n");
		para_msg_reply(&ri);
	} else {
		printf(" W < nothing to do.\n");
	}

	if (quit)
		printf(" W < signalled to quit.\n");

	return quit;
}


__pc_module para_mod_body(int)
{
	int msgid;

	// sleep to let the loader print everything...
	sleep(1);

	printf(" W < hello, world.  using message \"%s\".\n", msg);

	if ((msgid = para_msg_subscribe(msg)) < 0) {
		printf(" W < could not subscribe to the message (error %d), dead.\n", msgid);
		return;
	}

	printf(" W < message code is: %x.\n", msgid);

	while (phase1(msgid) == false)
		;

	printf(" W < end.\n");
}
