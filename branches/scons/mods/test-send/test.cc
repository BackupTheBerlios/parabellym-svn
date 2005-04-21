// parafile: simple profile storage for Parabellym
// Copyright (C) 2003 hex@faerion.oss and others.
// See file LICENSE for details about usage and redistribution.
//
// $Id$
//
// The receiving part.

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

static void phase1(para_msgi_t &mi)
{
	int rc;
	char text[] = "a notification";
	para_iov_t att(text, strlen(text) + 1);

	mi.rreq = false;
	mi.attc = 1;
	mi.attv = &att;

	printf(" S > sending a notification (rreq=false).\n");
	if ((rc = para_msg_send(&mi)) < 0)
		printf(" S > sending failed with error %d (%s).\n", rc, para_errstr(rc));
	printf(" S > notification sent.\n");
}


static void phase2(para_msgi_t &mi)
{
	int rc;
	char text[] = "an empty request";
	para_iov_t att(text, strlen(text) + 1);

	mi.rreq = true;
	mi.attc = 1;
	mi.attv = &att;

	printf(" S > sending a request (rreq=true).\n");
	if ((rc = para_msg_send(&mi)) < 0)
		printf(" S > sending failed with error %d (%s).\n", rc, para_errstr(rc));
	else {
		if (mi.attc)
			printf(" S > response: attc=%u, tex=\"%s\".\n", mi.attc, reinterpret_cast<char *>(mi.attv[0].data));
		else
			printf(" S > response: empty.\n");
	}
}


static void phase3(para_msgi_t &mi)
{
	int rc;
	char text[] = "a request";
	para_iov_t att(text, strlen(text) + 1);

	mi.rreq = true;
	mi.attc = 1;
	mi.attv = &att;

	printf(" S > sending a request (rreq=true).\n");
	if ((rc = para_msg_send(&mi)) < 0)
		printf(" S > sending failed with error %d (%s; %d).\n", rc, para_errstr(rc), mi.msgid);
	else {
		if (mi.attc)
			printf(" S > response: attc=%u, tex=\"%s\".\n", mi.attc, reinterpret_cast<char *>(mi.attv[0].data));
		else
			printf(" S > response: empty.\n");
	}
}


static void phase4(para_msgi_t &mi)
{
	int rc;
	char text[] = "end";
	para_iov_t att(text, strlen(text) + 1);

	mi.rreq = false;
	mi.attc = 1;
	mi.attv = &att;

	printf(" S > sending a termination request.\n");
	if ((rc = para_msg_send(&mi)) < 0)
		printf(" S > sending failed with error %d (%s).\n", rc, para_errstr(rc));
	printf(" S > notification sent.\n");
}


void para_mod_body(int)
{
	para_msgi_t info;

	// sleep to let the loader print everything...
	sleep(1);

	printf(" S > hello, world.  using message \"%s\".\n", msg);

	for (info.msgid = para_msg_find(msg); info.msgid < 0; info.msgid = para_msg_find(msg)) {
		printf(" S > message not found, throttling.\n");
		sleep(1);
	}

	printf(" S > message found with code %x.\n", info.msgid);

	phase1(info);
	phase2(info);
	phase3(info);
	phase4(info);

	printf(" S > end.\n");
}
