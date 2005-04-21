// parafile: simple profile storage for Parabellym
// Copyright (C) 2003 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// A self-testing module.  Spawns several threads, sends
// several signals.

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#if defined(_WIN32)
# include <windows.h>
# define sleep(x) Sleep(x * 1000)
#else
# include <unistd.h>
#endif
#include <faeutil/sem.h>
#include "../../core/api.h"

#define CHECK(cmd, pfx, cmt) if ((rc = cmd) != PEC_SUCCESS) printf(" " pfx " - error " cmt ": %s.\n", para_errstr(rc))

static unsigned int children = 0;
static faeutil::sem csem;
static const int sigend = 123;

static void* slave(void *_qid)
{
	int rc, qid = * reinterpret_cast<int *>(_qid);

	if ((rc = para_msg_attach(qid)) != PEC_SUCCESS) {
		printf(" S - error attaching to queue: %s.\n", para_errstr(rc));
		return NULL;
	}

	while (true) {
		para_msgi_t mi;

		if ((rc = para_msg_receive(&mi)) == PEC_SIGNAL && mi.signal == sigend) {
			printf(" S < received the unload signal.\n");
			para_msg_signal(qid, sigend);
			--children;
			csem.post();
			return NULL;
		}

		printf(" S < received an unknown action, ignoring.\n");
	}
}

void para_mod_body(int qid)
{
	pthread_t thread;

	// sleep to let the loader print everything...
	sleep(1);

	for (int idx = 0; idx < 2; ++idx) {
		if (pthread_create(&thread, NULL, slave, &qid) != 0)
			printf(" M - could not spawn a slave thread.\n");
		else {
			printf(" M - spawned a slave thread.\n");
			++children;
		}
	}

	printf(" M - hello, world.\n");
	printf(" M - waiting for signals.\n");

	while (true) {
		int rc;
		para_msgi_t mi;

		switch (rc = para_msg_receive(&mi)) {
		case PEC_SUCCESS:
			printf(" M < received a message -- unexpected.\n");
			break;
		case PEC_SIGNAL:
			if (mi.signal == PSIG_UNLOAD)
				printf(" M < received the unload signal.\n");
			else
				printf(" M < received an unknown signal.\n");
			break;
		default:
			printf(" M < unknown receive error: %d.\n", rc);
			break;
		}

		if (rc != PEC_SIGNAL || mi.signal != PSIG_UNLOAD)
			continue;

		printf(" M > sending PSIG_UNLOAD.\n");
		if ((rc = para_msg_signal(qid, PSIG_UNLOAD)) == PEC_SUCCESS)
			printf(" M ? success -- this is WRONG.\n");
		else
			printf(" M - failed (\"%s\"), this is correct.\n", para_errstr(rc));

		printf(" M - now sending a custom unload signal (%d) to queue %d.\n", sigend, qid);
		if ((rc = para_msg_signal(qid, sigend)) != PEC_SUCCESS)
			printf(" M ? failed: %s.\n", para_errstr(rc));
		else {
			printf(" M - success, good.\n");
			break;
		}
	}

	printf(" M - waiting for children to quit.\n");

	while (true) {
		faeutil::mlock lock(csem.mx);
		if (children == 0) {
			printf(" M - all children have quit.\n");
			break;
		} else {
			printf(" M - %u children left.\n", children);
		}
		csem.waitex(true);
	}

	printf(" M > exiting.\n");
}
